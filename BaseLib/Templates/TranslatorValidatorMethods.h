#ifndef BaseLib_Templates_TranslatorValidatorMethods_h_Included
#define BaseLib_Templates_TranslatorValidatorMethods_h_Included

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Templates/StrategyMethods.h"

namespace BaseLib
{
namespace Templates
{

// --------------------------------------------
// Validation methods
// --------------------------------------------

template <typename Return>
class IsValidMethod
{
public:
    virtual ~IsValidMethod()
    { }

    virtual Return IsValid() const = 0;
};

template <typename Return>
class IsValidatedMethod
{
public:
    virtual ~IsValidatedMethod()
    { }

    virtual Return IsValidated() const = 0;
};

template <typename Return>
class ValidateMethod
{
public:
    virtual ~ValidateMethod()
    { }

    virtual Return Validate() const = 0;
};

template <typename Return>
class PreValidateMethod
{
public:
    virtual ~PreValidateMethod()
    { }

    virtual Return PreValidate() const = 0;
};

template <typename Return>
class PostValidateMethod
{
public:
    virtual ~PostValidateMethod()
    { }

    virtual Return PostValidate() const = 0;
};

// --------------------------------------------
// Translation methods
// --------------------------------------------

template <typename Return>
class TranslateMethod
{
public:
    virtual ~TranslateMethod()
    { }

    virtual Return Translate() const = 0;
};


// --------------------------------------------
// TranslatorTranslator methods
// --------------------------------------------


template <typename T, typename K>
class TranslatorValidator : public TranslateMethod<T>, ValidateMethod<K>
{
public:
    template <typename Arg1>
    TranslatorValidator(std::shared_ptr<StrategyConst1<T, Arg1>> translate,
                        std::shared_ptr<StrategyConst1<K, Arg1>> validate,
                        const Arg1& arg1)
        : translate_(new StrategyConst1Invoker<T, Arg1>(translate, arg1))
        , validate_(new StrategyConst1Invoker<T, Arg1>(validate, arg1))
    { }

    template <typename Arg1, typename Arg2>
    TranslatorValidator(std::shared_ptr<StrategyConst2<T, Arg1, Arg2>> translate,
                        std::shared_ptr<StrategyConst2<K, Arg1, Arg2>> validate,
                        const Arg1& arg1,
                        const Arg2& arg2)
        : translate_(new StrategyConst2Invoker<T, Arg1, Arg2>(translate, arg1, arg2))
        , validate_(new StrategyConst2Invoker<K, Arg1, Arg2>(validate, arg1, arg2))
    { }

//    template <typename Arg1>
//    TranslatorValidator(std::function<T (Arg1)> translate,
//                        std::function<K (Arg1)> validate,
//                        const Arg1& arg1)
//        : translate_(new StrategyConst1Invoker(translate, arg1))
//        , validate_(new StrategyConst1Invoker(validate, arg1))
//    { }
//
//    template <typename Arg1, typename Arg2>
//    TranslatorValidator(std::function<T (Arg1, Arg2)> translate,
//                        std::function<K (Arg1, Arg2)> validate,
//                        const Arg1& arg1,
//                        const Arg2& arg2)
//        : translateFun_(translate)
//        , validateFun_(validate)
//    {
//        std::mem_fn(translate);
//    }

    virtual T Translate() const
    {
        translate_->Perform();
    }

    virtual K Validate() const
    {
        validate_->Perform();
    }

private:
    std::shared_ptr<Strategy0<T>> translate_;
    std::shared_ptr<Strategy0<K>> validate_;

//    auto translateFun_;
//    auto validateFun_;
};

}}

#endif
