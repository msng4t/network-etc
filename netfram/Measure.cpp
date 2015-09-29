#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include <stdio.h>
#include "Log.h"

#include <sstream>

#include <iostream>
#include <string>
#include "Measure.h"
#include <string.h>
#include "SockAddr.h"

/* YYMS for watch and performance analysis */
/* by lichong  2011-04-19 */
#ifndef WIN32
#include <stdio.h>
#include <map>
#include <sys/time.h>
#include "Synch.h"

MINA_NAMESPACE_START
CLock measureMutex;

char *uri2str(uint32_t uri)
{
  uint32_t uri_h = uri/256;
  uint32_t uri_t = uri % 256;

  static char s[16];
  sprintf(s, "%3d|%3d", uri_h, uri_t);
  return s;
}

typedef struct measureData{
	measureData(){		
		callTimes = 0;
		costTime = 0;
		maxTime = 0;
		allCostTime = 0;
		allCallTime = 0;
		allMaxTime = 0;
	}
	int callTimes;
	uint64_t costTime;
    uint64_t maxTime;
	uint64_t allCostTime;
	uint64_t allCallTime;
	uint64_t allMaxTime;
} measureData_t;

typedef std::map<URI_TYPE, measureData_t> uriTimeCount_t;
typedef std::map<uint32_t, measureData_t> IpTimeCount_t;
typedef std::map<uint32_t, measureData_t> ServerIdTimeCount_t;
static bool bEnableMeasure = false;
static bool bEnableFrom = false;
static bool bEnableFromIp = false;
static uriTimeCount_t uriTimeCount;
static IpTimeCount_t IpTimeCount;
static ServerIdTimeCount_t ServerIdTimeCount;
static double cpu_freq = 1; /*MHz*/
static double cpu_freq_magnification = 1; /*MHz*/
static bool bIsInit = false;

bool measureMultiThread = false;
uint32_t measureActiveThread = 0;
uint32_t measureDropedRequest = 0;
uint32_t measureQueueSize = 0;

void measureEnableMultiThread()
{
	measureMultiThread = true;
}

void measureSetQueueSize(uint32_t n)
{
	measureQueueSize = n;
}

uint32_t measureGetQueueSize()
{
	return measureQueueSize;
}

void measureSetActiveThread(uint32_t n)
{
	measureActiveThread = n;
}
void measureSetDropedRequest(uint32_t n)
{
	measureDropedRequest = n;
}

scoped_measure::scoped_measure(URI_TYPE uri, uint32_t ip, uint32_t serverId)
{
	if(bEnableMeasure == false){
		return;
	}
	if( !bIsInit )
	{
		bIsInit = true;
		init_cpu_freq();
	}
	m_startTime = get_cpu_cycle();
	m_uri = uri;
	m_ip = ip;
	m_serverId = serverId;
}

uint64_t scoped_measure::count_cycles()
{	
	return get_tick_interval(m_startTime, get_cpu_cycle()); 
}

scoped_measure::~scoped_measure()
{	
	if(bEnableMeasure == false){
		return;
	}

	uint64_t costTime = count_cycles(); 
	
   TSmartLock<CLock> loGuard(measureMutex);
	
	{//URI
		measureData_t &md = uriTimeCount[m_uri];
		md.costTime += costTime;
		md.allCostTime += costTime;
		md.callTimes += 1;
		md.allCallTime += 1;
		if(costTime > md.maxTime){
			 md.maxTime = costTime;
		}
		if(costTime > md.allMaxTime){
			 md.allMaxTime = costTime;
		}
	}
	if(bEnableFrom && m_serverId != 0){//ServerId
		measureData_t &md = ServerIdTimeCount[m_serverId];
		md.costTime += costTime;
		md.allCostTime += costTime;
		md.callTimes += 1;
		md.allCallTime += 1;
		if(costTime > md.maxTime){
			 md.maxTime = costTime;
		}
		if(costTime > md.allMaxTime){
			 md.allMaxTime = costTime;
		}
	}else if(bEnableFromIp && m_ip != 0){//IP
		measureData_t &md = IpTimeCount[m_ip];
		md.costTime += costTime;
		md.allCostTime += costTime;
		md.callTimes += 1;
		md.allCallTime += 1;
		if(costTime > md.maxTime){
			 md.maxTime = costTime;
		}
		if(costTime > md.allMaxTime){
			 md.allMaxTime = costTime;
		}
	}
}

uint64_t scoped_measure::count_us()
{	
	return get_usec_interval(0, count_cycles());
}



/* when dump, print one line with the cost each uri process used, and reset to zero */
void measureDump(bool getFrom, bool getFromIp)
{
	bEnableMeasure = true;
	bEnableFrom = getFrom;
	bEnableFromIp = getFromIp;
   TSmartLock<CLock> loGuard(measureMutex);

	uriTimeCount_t::iterator iter;
	for(iter = uriTimeCount.begin(); iter != uriTimeCount.end(); iter++){
		measureData_t &md = iter->second;
		uint64_t costTime = md.costTime;
		if(costTime > 0){
			uint32_t uri_h = iter->first >> 8;
			uint32_t uri_t = iter->first & 0x000000ff;
			LOG_TRACE("measure: %4u|%3u = %8" PRIu64 ", %7u, %7" PRIu64 ", %7" PRIu64 "", uri_h, uri_t,
				get_usec_interval(0, costTime),
				md.callTimes, 
				get_usec_interval(0, costTime/md.callTimes), 
				get_usec_interval(0, md.maxTime));
		}

		md.callTimes = 0;
		md.costTime = 0;
        md.maxTime = 0;
	}
}

std::string measureDumpJSON(bool getFrom, bool getFromIp, bool getAndClear)
{
	std::ostringstream os;
	bEnableMeasure = true;
	bEnableFrom = getFrom;
	bEnableFromIp = getFromIp;
   TSmartLock<CLock> loGuard(measureMutex);


	os << "	\"stat\":" << std::endl;
	os << "	{" << std::endl;
	
	if(measureMultiThread){
		os << "		\"queue\":" << std::endl;
		os << "		{" << std::endl;
		os << "			\"queue\":\"" << measureQueueSize << "\"," << std::endl;
		os << "			\"activeThread\":" << measureActiveThread << "," << std::endl;
		os << "			\"dropedRequest\":" << (measureDropedRequest) << std::endl;
		os << "		},";
	}

	{//URI
		bool firstOutputElement = true;

		os << "		\"uri\":" << std::endl << "		[" << std::endl;
		uriTimeCount_t::iterator iter;
		for(iter = uriTimeCount.begin(); iter != uriTimeCount.end(); ++iter){
			if(iter->second.allCallTime == 0)	continue;
			if(firstOutputElement == false){
				os << "," << std::endl;
			}else{
				firstOutputElement = false;
				os << std::endl;
			}
			os << "			{" << std::endl;
			os << "				\"uri\":\"" << uri2str(iter->first) << "\"," << std::endl;
			os << "				\"counter64\":" << (iter->second.allCallTime) << "," << std::endl;
			os << "				\"time_cost\":" << (get_usec_interval(0, iter->second.allCostTime)) << "," << std::endl;
			os << "				\"max_time\":" << (get_usec_interval(0, iter->second.allMaxTime)) << std::endl;
			os << "			}";
			if( getAndClear){
				iter->second.allCostTime = 0;
				iter->second.allCallTime = 0;
				iter->second.allMaxTime = 0;
			}
		}
		os << std::endl << "		]," << std::endl;
	}
	
	if(bEnableFromIp){//IP
		bool firstOutputElement = true;

		os << "		\"ip_src\":" << std::endl << "		[" << std::endl;
		IpTimeCount_t::iterator iter;
		for(iter = IpTimeCount.begin(); iter != IpTimeCount.end();){
			if(firstOutputElement == false){
				os << "," << std::endl;
			}else{
				firstOutputElement = false;
				os << std::endl;
			}
			os << "			{" << std::endl;
			os << "				\"ip\":\"" << CSockAddr::GetIP(iter->first) << "\"," << std::endl;
			os << "				\"counter64\":" << (iter->second.allCallTime) << "," << std::endl;
			os << "				\"time_cost\":" << (get_usec_interval(0, iter->second.allCostTime)) << "," << std::endl;
			os << "				\"max_time\":" << (get_usec_interval(0, iter->second.allMaxTime)) << std::endl;
			os << "			}";
			if( getAndClear){
				IpTimeCount.erase(iter++);
			}else{
				 ++iter;
			}
		}
		os << std::endl << "		]," << std::endl;
	}
	if(bEnableFrom){//ServerId
		bool firstOutputElement = true;

		os << "		\"server_id_src\":" << std::endl << "		[" << std::endl;
		ServerIdTimeCount_t::iterator iter;
		for(iter = ServerIdTimeCount.begin(); iter != ServerIdTimeCount.end();){
			if(firstOutputElement == false){
				os << "," << std::endl;
			}else{
				firstOutputElement = false;
				os << std::endl;
			}
			os << "			{" << std::endl;
			os << "				\"server_id\":" << CSockAddr::GetIP(iter->first) << "," << std::endl;
			os << "				\"counter64\":" << (iter->second.allCallTime) << "," << std::endl;
			os << "				\"time_cost\":" << (get_usec_interval(0, iter->second.allCostTime)) << "," << std::endl;
			os << "				\"max_time\":" << (get_usec_interval(0, iter->second.allMaxTime)) << std::endl;
			os << "			}";
			if( getAndClear){
				ServerIdTimeCount.erase(iter++);
			}else{
				 ++iter;
			}
		}
		os << std::endl << "		]" << std::endl;
	}
	os << "	}";
	return std::string(os.str());
}

uint64_t get_cpu_cycle()
{
  union cpu_cycle{
    struct t_i32 {
      uint32_t l;
      uint32_t h;
    } i32;
    uint64_t t;
  } c;

  rdtsc( c.i32.l, c.i32.h );

  return c.t;
}
uint64_t get_usec_interval( uint64_t start, uint64_t stop )
{
  if( stop < start)
    return 0;
  return (uint64_t)((stop - start) / cpu_freq);
}

uint32_t get_msec_interval( uint64_t start, uint64_t stop )
{
  if( stop < start)
    return 0;
  return (uint32_t)((stop - start) / cpu_freq_magnification);
}
uint64_t get_tick_interval(uint64_t start, uint64_t stop)
{
  if( stop < start)
    return 0;
  return (stop - start);
}
int init_cpu_freq()
{
  FILE * fp;
  char * str;
  const char * cmd;
  int ratio = 1;

  str = (char*)malloc(1024);

  fp = popen( "cat /proc/cpuinfo | grep -m 1 \"model name\"", "r" );
  //char* p = fgets( str, 1024, fp );
  fgets(str, 1024, fp);
  fclose( fp );

  if( strstr( str, "AMD" ) )
  {
    cmd =  "cat /proc/cpuinfo | grep -m 1 \"cpu MHz\" | sed -e \'s/.*:[^0-9]//\'";
  }
  else
  {
    cmd = "cat /proc/cpuinfo | grep -m 1 \"model name\" | sed -e \"s/^.*@ //g\" -e \"s/GHz//g\"";
    ratio = 1000;
  }

  fp = popen( cmd, "r" );
  if( fp == NULL )
  {
    LOG_ERROR("get cpu info failed\n");
    return -1;
  }
  else
  {
    //p = 
	fgets( str, 1024, fp );
    cpu_freq = atof(str) * ratio;
    cpu_freq_magnification = cpu_freq*1000;
    fclose( fp );
    LOG_TRACE("get cpu info cpu_freq %f cpu_freq_magnification %f ", cpu_freq, cpu_freq_magnification);
  }

  free( str );

  return 0;
}

MINA_NAMESPACE_END
#endif
//End of YYMS

