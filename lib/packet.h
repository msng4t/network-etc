
#ifndef _PACKET_H_INCLUDE__
#define _PACKET_H_INCLUDE__

#include "int_types.h"
#include "blockbuffer.h"
#include "varstr.h"
#include "header.h"
#include "utility.h"

#include <string>
#include <iostream>
#include <stdexcept>
#include <map> // CARE
#include <bitset>

namespace sox{
  class PackBufferX;
  class PackX;
  class UnpackX;
}

namespace sox{


struct PacketError : public std::runtime_error
{
	PacketError(const std::string & w) :
	std::runtime_error(w)
	{
	}
};

struct PackError : public PacketError
{
	PackError(const std::string & w) :
	PacketError(w)
	{
	}
};

struct UnpackError : public PacketError
{
	UnpackError(const std::string & w) :
	PacketError(w)
	{
	}
};

class PackBuffer
{
  friend class sox::PackBufferX;
private:
	// use big-block. more BIG? MAX 64K*16k = 1G
  typedef BlockBuffer<def_block_alloc_16k, 65536> BB;
  BB bb;
public:
	char * data()
	{
		return bb.data();
	}
	size_t size() const
	{
		return bb.size();
	}

	void resize(size_t n)
	{
		if(bb.resize(n))
			return;
		throw PackError("resize buffer overflow");
	}
	void append(const char * data, size_t size)
	{
		if(bb.append(data, size))
			return;
		throw PackError("append buffer overflow");
	}
	void append(const char * data)
	{
		append(data,:: strlen(data));
	}
	void replace(size_t pos, const char * rep, size_t n)
	{
		if(bb.replace(pos, rep, n))	return;
		throw PackError("replace buffer overflow");
	}
	void reserve(size_t n)
	{
		if(bb.reserve(n)) return;
		throw PackError("reserve buffer overflow");
	}

};

#if defined(__i386__)||defined(WIN32) || defined(__x86_64__)

	#define XHTONS
	#define XHTONL
	#define XHTONLL

#else /* big end */

inline uint16_t XHTONS(uint16_t i16)
{
	return((i16 << 8) | (i16 >> 8));
}
inline uint32_t XHTONL(uint32_t i32)
{
	return((uint32_t(XHTONS(i32)) << 16) | XHTONS(i32>>16));
}
inline uint64_t XHTONLL(uint64_t i64)
{
	return((uint64_t(XHTONL((uint32_t)i64)) << 32) |XHTONL((uint32_t(i64>>32))));
}

#endif /* __i386__ */

#define XNTOHS XHTONS
#define XNTOHL XHTONL
#define XNTOHLL XHTONLL

class Pack
{
  friend class sox::PackX;
private:
	Pack (const Pack & o);
	Pack & operator = (const Pack& o);
public:
	uint16_t xhtons(uint16_t i16)
	{
		return XHTONS(i16);
	}
	uint32_t xhtonl(uint32_t i32)
	{
		return XHTONL(i32);
	}
	uint64_t xhtonll(uint64_t i64)
	{
		return XHTONLL(i64);
	}

	// IMPORTANT remember the buffer-size before pack. see data(), size()
	// reserve a space to replace packet header after pack parameter
	// sample below: OffPack. see data(), size()
	Pack(PackBuffer & pb, size_t off = 0) : m_buffer(pb)
	{
		m_offset = pb.size() + off;
		m_buffer.resize(m_offset);
	}

	// access this packet.
	char * data()
	{
		return m_buffer.data() + m_offset;
	}
	const char * data()  const
	{
		return m_buffer.data() + m_offset;
	}
	size_t size() const
	{
		return m_buffer.size() - m_offset;
	}

	Pack & push(const void * s, size_t n)
	{
		m_buffer.append((const char *)s, n); return *this;
	}
	Pack & push(const void * s)
	{
		m_buffer.append((const char *)s); return *this;
	}

	Pack & push_uint8(uint8_t u8)
	{
		return push(&u8, 1);
	}
	Pack & push_uint16(uint16_t u16)
	{
		u16 = xhtons(u16); return push(&u16, 2);
	}
	Pack & push_uint32(uint32_t u32)
	{
		u32 = xhtonl(u32); return push(&u32, 4);
	}
	Pack & push_uint64(uint64_t u64)
	{
		u64 = xhtonll(u64); return push(&u64, 8);
	}

	Pack & push_varstr(const Varstr & vs)
	{
		return push_varstr(vs.data(), vs.size());
	}
	Pack & push_varstr(const void * s)
	{
		return push_varstr(s, strlen((const char *)s));
	}
	Pack & push_varstr(const std::string & s)
	{
		return push_varstr(s.data(), s.size());
	}
	Pack & push_varstr(const void * s, size_t len)
	{
		if(len > 0xFFFF) throw PackError("push_varstr: varstr too big");
		return push_uint16(uint16_t(len)).push(s, len);
	}
	Pack & push_varstr32(const void * s, size_t len)
	{
		if(len > 0xFFFFFFFF) throw PackError("push_varstr32: varstr too big");
		return push_uint32(uint32_t(len)).push(s, len);
	}
#ifdef WIN32
	Pack & push_varwstring32(const std::wstring &ws)
	{
		size_t len = ws.size() * 2;
		return push_uint32((uint32_t)len).push(ws.data(), len);
	}
#endif
	/*
	Pack & push_octlen(size_t len)
	{
		uint8_t oct[sizeof(len)];
		int i = sizeof(len);

		// 0.7, <=127 //
		// 1.7-0.7 <= 16k //
		// 1.7-1.7-0.7 < 2^21
		do
		{
			oct[--i] = len & 0x7f;
			len >>= 7;
		} while (len > 0);

		for ( ; i < sizeof(len) - 1; ++i)
			push_uint8(oct[i]|0x80);
		push_uint8(oct[i]);
	}
	struct octstr
	{
		octstr(const std::string & s) : str(s) { }
	};
	size_t pop_octlen()
	{
		size_t len = 0;
		uint8_t oct;
		do
		{
			oct = pop_uint8();
			len <<= 7;
			len |= (oct & 0x7f);
		} while (oct > 0x7f);
		return len;
	}

	Pack & push_octstr(const Varstr & vs)          { return push_octstr(vs.data(), vs.size()); }
	Pack & push_octstr(const void * s)             { return push_octstr(s, strlen((const char *)s)); }
	Pack & push_octstr(const std::string & s)      { return push_octstr(s.data(), s.size()); }
	Pack & push_octstr(const void * s, size_t len) { return push_octlen(len).push(s, len); }
	*/

	virtual ~Pack()
	{
	}
public:
	// replace. pos is the buffer offset, not this Pack m_offset
	size_t replace(size_t pos, const void * data, size_t rplen)
	{
		m_buffer.replace(pos, (const char*)data, rplen);
		return pos + rplen;
	}
	size_t replace_uint8(size_t pos, uint8_t u8)
	{
		return replace(pos, &u8, 1);
	}
	size_t replace_uint16(size_t pos, uint16_t u16)
	{
		u16 = xhtons(u16);
		return replace(pos, &u16, 2);
	}
	size_t replace_uint32(size_t pos, uint32_t u32)
	{
		u32 = xhtonl(u32);
		return replace(pos, &u32, 4);
	}

	// replace_offset. pos is a relative offset of the buffer:
	// real pos of m_buffer = pos + original offset of this Pack (m_offset)
	size_t replace_offset(size_t pos, const void * data, size_t rplen) {
		m_buffer.replace(pos + m_offset, (const char*)data, rplen);
		return pos + rplen;
	}
	size_t replace_offset_uint8(size_t pos, uint8_t u8)    { 
		return replace_offset(pos, &u8, 1); 
	}
	size_t replace_offset_uint16(size_t pos, uint16_t u16) {
		u16 = xhtons(u16);
		return replace_offset(pos, &u16, 2);
	}
	size_t replace_offset_uint32(size_t pos, uint32_t u32) {
		u32 = xhtonl(u32);
		return replace_offset(pos, &u32, 4);
	}	
protected:
	PackBuffer & m_buffer;
	size_t m_offset;
};

/* sample
struct OffPack : public Pack
{
	enum { BYTES = 12 };
	OffPack(PackBuffer &pb) : Pack(pb, BYTES)
	{
	}
	void endpack()
	{
		//assert(m_offset >= BYTES);
		m_offset -= BYTES;

		// use relace method to fill the reserve space
		size_t rppos = m_offset; // beginning of the reserve-space
		rppos = replace_uint32(rppos, 1); // 4 bytes
		rppos = replace(rppos, "123456", 6); // 6 bytes
		rppos = relpace_uint16(rppos, 2); // 2 bytes
	}
};
*/

class Unpack
{
  friend class sox::UnpackX;
public:
	uint16_t xntohs(uint16_t i16) const
	{
		return XNTOHS(i16);
	}
	uint32_t xntohl(uint32_t i32) const
	{
		return XNTOHL(i32);
	}
	uint64_t xntohll(uint64_t i64) const
	{
		return XNTOHLL(i64);
	}

	Unpack(const void * data, size_t size)
	{
		reset(data, size);
	}
	void reset(const void * data, size_t size) const
	{
		m_data = (const char *)data;
		m_size = size;
	}

	virtual ~Unpack()
	{
		m_data = NULL;
	}

	operator const void *() const
	{
		return m_data;
	}
	bool operator!() const
	{
		return(NULL == m_data);
	}

	std::string pop_varstr() const
	{
		Varstr vs = pop_varstr_ptr();
		return std::string(vs.data(), vs.size());
	}

	std::string pop_varstr32() const
	{
		Varstr vs = pop_varstr32_ptr();
		return std::string(vs.data(), vs.size());
	}
#ifdef WIN32
	std::wstring pop_varwstring32() const
	{
		Varstr vs = pop_varstr32_ptr();
		return std::wstring((wchar_t *)vs.data(), vs.size() / 2);
	}
#endif
	std::string pop_fetch(size_t k) const
	{
		return std::string(pop_fetch_ptr(k), k);
	}

	void finish() const
	{
		if(!empty()) throw UnpackError("finish: too much data");
	}

	uint8_t pop_uint8() const
	{
		if(m_size < 1u)
			throw UnpackError("pop_uint8: not enough data");

		uint8_t i8 = *((uint8_t*)m_data);
		m_data += 1u; m_size -= 1u;
		return i8;
	}

	uint16_t pop_uint16() const
	{
		if(m_size < 2u)
			throw UnpackError("pop_uint16: not enough data");

		uint16_t i16 = *((uint16_t*)m_data);
		i16 = xntohs(i16);

		m_data += 2u; m_size -= 2u;
		return i16;
	}

	uint32_t pop_uint32() const
	{
		if(m_size < 4u)
			throw UnpackError("pop_uint32: not enough data");
		uint32_t i32 = *((uint32_t*)m_data);
		i32 = xntohl(i32);
		m_data += 4u; m_size -= 4u;
		return i32;
	}

	/**
	 * 为锟斤拷锟斤拷锟斤拷unmarshal_container时锟斤拷取锟斤拷锟斤拷锟斤拷锟斤拷size锟斤拷锟斤拷reserve锟节达拷锟斤拷锟斤拷
	 */
	uint32_t peek_uint32() const {
		if (m_size < 4u)
			throw UnpackError("peek_uint32: not enough data");
		uint32_t i32 = *((uint32_t*)m_data);
		i32 = xntohl(i32);
		return i32;
	}
	uint64_t pop_uint64() const
	{
		if(m_size < 8u)
			throw UnpackError("pop_uint64: not enough data");
		uint64_t i64 = *((uint64_t*)m_data);
		i64 = xntohll(i64);
		m_data += 8u; m_size -= 8u;
		return i64;
	}

	Varstr pop_varstr_ptr() const
	{
		// Varstr { uint16_t size; const char * data; }
		Varstr vs;
		vs.m_size = pop_uint16();
		vs.m_data = pop_fetch_ptr(vs.m_size);
		return vs;
	}

	Varstr pop_varstr32_ptr() const
	{
		Varstr vs;
		vs.m_size = pop_uint32();
		vs.m_data = pop_fetch_ptr(vs.m_size);
		return vs;
	}

	const char * pop_fetch_ptr(size_t k) const
	{
		if(m_size < k)
		{
			//abort();
			throw UnpackError("pop_fetch_ptr: not enough data");
		}

		const char * p = m_data;
		m_data += k; m_size -= k;
		return p;
	}

	bool empty() const
	{
		return m_size == 0;
	}
	const char * data() const
	{
		return m_data;
	}
	size_t size() const
	{
		return m_size;
	}

private:
	mutable const char * m_data;
	mutable size_t m_size;
};

struct Marshallable 
{
	virtual void marshal(Pack &) const = 0;
	virtual void unmarshal(const Unpack &) = 0;
	virtual ~Marshallable()
	{
	}
	virtual std::ostream & trace(std::ostream & os) const
	{
		return os << "trace Marshallable [ not immplement ]";
	}
	virtual std::string dump() const
	{
		std::ostringstream os;
		trace(os);
		return os.str();
	}
};

// Marshallable helper
inline std::ostream & operator << (std::ostream & os, const Marshallable & m)
{
	return m.trace(os);
}

inline Pack & operator << (Pack & p, const Marshallable & m)
{
	m.marshal(p);
	return p;
}

inline const Unpack & operator >> (const Unpack & p, const Marshallable & m)
{
	const_cast<Marshallable &>(m).unmarshal(p);
	return p;
}

struct Voidmable : public Marshallable
{
	virtual void marshal(Pack &) const
	{
	}
	virtual void unmarshal(const Unpack &)
	{
	}
};

struct Mulmable : public Marshallable
{
	Mulmable(const Marshallable & m1, const Marshallable & m2)
	: mm1(m1), mm2(m2)
	{
	}

	const Marshallable & mm1;
	const Marshallable & mm2;

	virtual void marshal(Pack &p) const
	{
		p << mm1 << mm2;
	}
	virtual void unmarshal(const Unpack &p)
	{
		assert(false);
	}
	virtual std::ostream & trace(std::ostream & os) const
	{
		return os << mm1 << mm2;
	}
};

struct Mulumable : public Marshallable
{
	Mulumable(Marshallable & m1, Marshallable & m2)
	: mm1(m1), mm2(m2)
	{
	}

	Marshallable & mm1;
	Marshallable & mm2;

	virtual void marshal(Pack &p) const
	{
		p << mm1 << mm2;
	}
	virtual void unmarshal(const Unpack &p)
	{
		p >> mm1 >> mm2;
	}
	virtual std::ostream & trace(std::ostream & os) const
	{
		return os << mm1 << mm2;
	}
};

struct Rawmable : public Marshallable
{
	Rawmable(const char * data, size_t size) : m_data(data), m_size(size)
	{
	}

	template < class T >
	explicit Rawmable(T & t ) : m_data(t.data()), m_size(t.size())
	{
	}

	const char * m_data;
	size_t m_size;

	virtual void marshal(Pack & p) const
	{
		p.push(m_data, m_size);
	}
	virtual void unmarshal(const Unpack &)
	{
		assert(false);
	}
	//virtual std::ostream & trace(std::ostream & os) const { return os.write(m_data, m_size); }
};

struct Marshallable2: public sox::Marshallable{
  /*
  +--------------+---------------------------+
  | 4 Bytes      | sizeof(data) Bytes        |
  +--------------+---------------------------+
  | sizeof(data) | binary data of PMessage   |
  +--------------+---------------------------+
  */

  virtual void marshal2(sox::Pack &pk) const = 0;
  virtual void unmarshal2(const sox::Unpack &up) = 0;

  virtual void marshal(sox::Pack &pk) const {
    size_t pos = pk.size();
    pk.push_uint32(0);
    marshal2(pk);
    uint32_t len = pk.size() - pos - sizeof(uint32_t);
    pk.replace_offset_uint32(pos, len); 
  }

  virtual void unmarshal(const sox::Unpack &up) {
    uint32_t len = up.pop_uint32();
    sox::Unpack inner(up.pop_fetch_ptr(len), len);
    unmarshal2(inner);
  }    
};

// base type helper
inline Pack & operator << (Pack & p, bool sign)
{
	p.push_uint8(sign ? 1 : 0);
	return p;
}

// base type helper
inline Pack & operator << (Pack & p, uint8_t i8)
{
	p.push_uint8(i8);
	return p;
}

inline Pack & operator << (Pack & p, uint16_t  i16)
{
	p.push_uint16(i16);
	return p;
}

inline Pack & operator << (Pack & p, uint32_t  i32)
{
	p.push_uint32(i32);
	return p;
}
inline Pack & operator << (Pack & p, uint64_t  i64)
{
	p.push_uint64(i64);
	return p;
}

inline Pack & operator << (Pack & p, const std::string & str)
{
	p.push_varstr(str);
	return p;
}
#ifdef WIN32
inline Pack & operator << (Pack & p, const std::wstring & str)
{
	p.push_varwstring32(str);
	return p;
}
#endif
inline Pack & operator << (Pack & p, const Varstr & pstr)
{
	p.push_varstr(pstr);
	return p;
}

inline const Unpack & operator >> (const Unpack & p, Varstr & pstr)
{
	pstr = p.pop_varstr_ptr();
	return p;
}

// pair.first helper
// XXX std::map::value_type::first_type unpack 需要特别定义
inline const Unpack & operator >> (const Unpack & p, uint32_t & i32)
{
	i32 =  p.pop_uint32();
	return p;
}

inline const Unpack & operator >> (const Unpack & p, uint64_t & i64)
{
	i64 =  p.pop_uint64();
	return p;
}

inline const Unpack & operator >> (const Unpack & p, std::string & str)
{
	// XXX map::value_type::first_type
	str = p.pop_varstr();
	return p;
}
#ifdef WIN32
inline const Unpack & operator >> (const Unpack & p, std::wstring & str)
{
	// XXX map::value_type::first_type
	str = p.pop_varwstring32();
	return p;
}
#endif
inline const Unpack & operator >> (const Unpack & p, uint16_t & i16)
{
	i16 =  p.pop_uint16();
	return p;
}
inline const Unpack & operator >> (const Unpack & p, uint8_t & i8)
{
	i8 =  p.pop_uint8();
	return p;
}
inline const Unpack & operator >> (const Unpack & p, bool & sign)
{
	sign =  (p.pop_uint8() == 0) ? false : true;
	return p;
}


template <class T1, class T2>
inline std::ostream& operator << (std::ostream& s, const std::pair<T1, T2>& p)
{
	s << p.first << '=' << p.second;
	return s;
}

template <class T1, class T2>
inline Pack& operator << (Pack& s, const std::pair<T1, T2>& p)
{
	s << p.first << p.second;
	return s;
}

template <class T1, class T2>
inline const Unpack& operator >> (const Unpack& s, std::pair<const T1, T2>& p)
{
	const T1& m = p.first;
	T1 & m2 = const_cast<T1 &>(m);
	s >> m2 >> p.second;
	return s;
}

template <class T1, class T2>
inline const Unpack& operator >> (const Unpack& s, std::pair<T1, T2>& p)
{
	s >> p.first >> p.second;
	return s;
}
/*
// vc . only need this
template <class T1, class T2>
inline const Unpack& operator>>(const Unpack& s, std::pair<T1, T2>& p)
{
	s >> p.first;
	s >> p.second;
	return s;
}
*/

template <class T>
inline Pack& operator << (Pack& p, const std::vector<T>& vec)
{
	marshal_container(p, vec);
	return p;
}

template <class T>
inline const sox::Unpack& operator >> (const sox::Unpack& p, std::vector<T>& vec)
{
	unmarshal_container(p, std::back_inserter(vec));
	return p;
}

template <class T>
inline Pack& operator << (Pack& p, const std::set<T>& set)
{
  marshal_container(p, set);
  return p;
}

template <class T>
inline const sox::Unpack& operator >> (const sox::Unpack& p, std::set<T>& set)
{
  unmarshal_container(p, std::inserter(set, set.begin()));
  return p;
}

template <class T1, class T2>
inline sox::Pack& operator << (sox::Pack& p, const std::map<T1, T2>& map)
{
  marshal_container(p, map);
  return p;
}

template <class T1, class T2>
inline const sox::Unpack& operator >> (const sox::Unpack& p, std::map<T1, T2>& map)
{
  unmarshal_container(p, std::inserter(map, map.begin()));
  return p;
}

template <size_t N>
inline Pack& operator << (Pack& p, const std::bitset<N>& bs)
{
	p.push_uint32(N);
	uint32_t pos = 1;
	uint32_t tmp = 0;
	// todo: low efficient
	for(uint32_t i = 0; i < N; ++i, ++pos)
	{
		tmp = (tmp << 1) | bs[i];
		if(sizeof(uint32_t)*8 == pos)
		{
			p.push_uint32(tmp);
			tmp = 0;
			pos = 0; // ++pos later
		}
	}
	if(pos > 1)
	{
		tmp <<= sizeof(uint32_t)*8 - pos + 1;
		p.push_uint32(tmp);
	}
	return p;
}

template <size_t N>
inline const sox::Unpack& operator >> (const sox::Unpack& p, std::bitset<N>& bs)
{
	uint32_t size = p.pop_uint32();
	for(uint32_t i=0; i*8*sizeof(uint32_t)<size; ++i)
	{
		uint32_t tmp = p.pop_uint32();
		uint32_t mask = 0x80000000;
		for(uint32_t j=i*8*sizeof(uint32_t); j<(i+1)*8*sizeof(uint32_t) && j<N; ++j)
		{
			bs[j] = tmp & mask;
			mask >>= 1;
		}
	}
	return p;
}

// container marshal helper
template < typename ContainerClass >
inline void marshal_container(Pack & p, const ContainerClass & c)
{
	p.push_uint32(uint32_t(c.size())); // use uint32 ...
	for(typename ContainerClass::const_iterator i = c.begin(); i != c.end(); ++i)
		p << *i;
}

template < typename OutputIterator >
inline void unmarshal_container(const Unpack & p, OutputIterator i)
{
	for(uint32_t count = p.pop_uint32(); count > 0; --count)
	{
		typename OutputIterator::container_type::value_type tmp;
		p >> tmp;
		*i = tmp;
		++i;
	}
}

//add by heiway 2005-08-08
//and it could unmarshal list,vector etc..
template < typename OutputContainer>
inline void unmarshal_containerEx(const Unpack & p, OutputContainer & c)
{
	for(uint32_t count = p.pop_uint32() ; count >0 ; --count)
	{
		typename OutputContainer::value_type tmp;
		tmp.unmarshal(p);
		c.push_back(tmp);
	}
}

template < typename ContainerClass >
inline std::ostream & trace_container(std::ostream & os, const ContainerClass & c, char div='\n')
{
	for(typename ContainerClass::const_iterator i = c.begin(); i != c.end(); ++i)
		os << *i << div;
	return os;
}

template<class T>
inline void packetToString(const T &objIn, std::string &strOut)
{
  PackBuffer buffer;
  Pack pack(buffer);
  objIn.marshal(pack);
  strOut.assign(pack.data(), pack.size());
}

template<class T>
inline bool stringToPacket(const std::string &strIn, T &objOut)
{
  try {
    Unpack unpack(strIn.data(), strIn.length());
    objOut.unmarshal(unpack);
  } catch (UnpackError e)
  {
    log(Error, "exception UnpackError, %s", (std::string("Inner Conn read error:") + e.what()).data() );
    return false;
  }
  return true;
}

struct CServiceDataEx2 : public Marshallable
{
  CServiceDataEx2():serviceId(0), serviceType(0), ispType(0),  groupId(0), serverPort(0)
  {
    m_uFrontPort=0;
    m_uUdpPort=0;
  }
  uint64_t serviceId; // service server's IP + console port.
  uint16_t serviceType; // 1 - third party icon; 2 - jifen
  uint32_t ispType;//N线使用MUTIL，单线使用单线的值
  uint16_t groupId;	  //机房ID
  uint16_t serverPort;  ////service间发现、互通新增字段,服务器间互通端口,只有此字段不为0时才会推送给感兴趣者
  std::map<uint32_t, std::string> extraData;
  uint16_t m_uFrontPort;//service给sproxy使用的端口
  uint16_t m_uUdpPort;//service给外部访问的udp端口
  std::map<uint32_t,uint32_t> m_mapIsp2Ip;//key:isp,value:ip地址
  void GetString(std::string & strData) const
  {
    std::stringstream ssTemp;
    std::string strIp;
    uint16_t uPort;
    utility::ServiceId2IpPort(serviceId,strIp,uPort);

    ssTemp<<"serviceid:"<< std::hex << serviceId;
    ssTemp<<std::dec;
    ssTemp<<",serviceip:"<<strIp<<",consoleport:"<<uPort;

    std::map<uint32_t,uint32_t>::const_iterator pIt=m_mapIsp2Ip.begin();
    for(;pIt!=m_mapIsp2Ip.end();pIt++)
    {
      ssTemp<<",isp:"<<pIt->first<<",ip:"<<utility::addr_ntoa(pIt->second);
    }
    ssTemp<<",port:"<<serverPort<<",groupid:"<<groupId;
    ssTemp<<",frontport:"<<m_uFrontPort<<",udpport:"<<m_uUdpPort;
    ssTemp<<",servicetype:"<<serviceType<<",isp:"<<ispType;
    for (std::map<uint32_t, std::string>::const_iterator it = extraData.begin();
      it != extraData.end(); ++it)
    {
      ssTemp << ",key:" << it->first << "value:" << it->second;
    }
    strData=ssTemp.str();
  }

  void marshal(Pack& pk) const
  {
    pk << serviceType << serverPort << serviceId << groupId<<m_uFrontPort<<m_uUdpPort;
    pk<<(ispType);
    marshal_container(pk, extraData);
    marshal_container(pk, m_mapIsp2Ip);
  }

  void unmarshal(const Unpack& up)
  {
    up >> serviceType >> serverPort >> serviceId >> groupId>>m_uFrontPort>>m_uUdpPort;
    up>>ispType;
    unmarshal_container(up, std::inserter(extraData, extraData.begin()));
    unmarshal_container(up, std::inserter(m_mapIsp2Ip,m_mapIsp2Ip.begin()));
  }
};

}

#endif // _SNOX_PACKET_H_INCLUDE__
