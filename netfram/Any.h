#ifndef _ANY_INCLUDE_
#define _ANY_INCLUDE_

#include <algorithm>
#include <typeinfo>
#include "Define.h"
#include <assert.h>

MINA_NAMESPACE_START

class CAny
{
public:
    CAny(void)
      : mpoContent(NULL) { }

    template<typename ValueType>
    CAny( ValueType const& atValue )
      : mpoContent(NULL)
    {
        this->mpoContent = new  CHolder<ValueType>(atValue);
        assert (this->mpoContent != NULL);
    }

    CAny( CAny const& aoOther )
      : mpoContent(aoOther.mpoContent ? aoOther.mpoContent->Clone() : NULL)
    {
    }

    ~CAny()
    {
        delete this->mpoContent;
    }

public:

    CAny& Swap( CAny& aoRhs )
    {
        std::swap( this->mpoContent, aoRhs.mpoContent );
        return *this;
    }

    template<typename ValueType>
    CAny& operator = (  ValueType const& aoRhs )
    {
        CAny(aoRhs).Swap(*this);
        return *this;
    }

    CAny& operator = ( const CAny& aoRhs )
    {
        CAny(aoRhs).Swap(*this);
        return *this;
    }

    bool IsEmpty(void) const
    {
        return this->mpoContent == NULL;
    }

    template<typename ValueType>
    ValueType* CastTo(void) const
    {
        return this->CastTo0( (ValueType*)0 );
    }

    template<typename ValueType>
    bool CastTo( ValueType& aoValue ) const
    {
        if (this->mpoContent == NULL || this->mpoContent->Type() != typeid(ValueType) )
        {
            return false;
        }
        else
        {
            aoValue = static_cast< CAny::CHolder<ValueType>* >(this->mpoContent)->mtHeld;
            return true;
        }
    }

protected:
    template<typename ValueType>
    ValueType* CastTo0( ValueType* ) const
    {
        if (this->mpoContent == NULL || this->mpoContent->Type() != typeid(ValueType) )
        {
            return NULL;
        }
        else
        {
            return &static_cast< CAny::CHolder<ValueType>* >(this->mpoContent)->mtHeld;
        }
    }

public:
    template<typename ValueType> friend inline ValueType* AnyCast( CAny const& aoOperand );
    template<typename ValueType> friend inline ValueType* AnyCast( CAny const* apoOperand );

private:
    class CPlaceHolder
    {
    public:
        virtual ~CPlaceHolder(){};

        virtual const std::type_info& Type(void) const = 0;

        virtual CPlaceHolder* Clone(void) const = 0;
    };

    template<typename ValueType>
    class CHolder : public CPlaceHolder
    {
    public:
        CHolder( const ValueType& aoValue )
            : mtHeld(aoValue)
        {
        }

        virtual const std::type_info& Type(void) const 
        {
            return typeid(this->mtHeld);
        }

        virtual CPlaceHolder* Clone() const
        {
            CHolder* lpoHolder = NULL;

            lpoHolder = new CHolder(this->mtHeld);
            assert (lpoHolder != NULL);

            return lpoHolder;
        }

    public:
        ValueType mtHeld;
    };

    CPlaceHolder* mpoContent;
};

template<typename ValueType> inline ValueType* AnyCast( CAny const& aoOperand )
{
    return aoOperand.CastTo0( (ValueType*)0 );
}

template<typename ValueType> inline ValueType* AnyCast( CAny const* apoOperand )
{
    return apoOperand != NULL ? apoOperand->CastTo0( (ValueType*)0 ) : NULL;
}

template<typename ValueType> inline bool AnyCast( CAny const& aoOperand, ValueType& aoValue )
{
    return aoOperand.CastTo(aoValue);
}


MINA_NAMESPACE_END
#endif
