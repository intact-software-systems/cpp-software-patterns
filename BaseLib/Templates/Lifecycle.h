#ifndef BaseLib_Templates_Lifecycle_h_Included
#define BaseLib_Templates_Lifecycle_h_Included

namespace BaseLib { namespace Templates
{

/*
 * IDEAS:
 *  - Automatically release this from parent when this goes out of scope. Use Finalize()
 *
 * this->Finalize() -> parent->Release(this);
 *
 *  - use cache instead and observer pattern?
 *
 * this->Finalize() -> cache()->SetExpired(this) -> notify parent through OnObjectDeleted(this).
 */

// --------------------------------------------
// Lifecycle methods
// --------------------------------------------

/**
 * @brief The InitMethod class
 */
class InitializeMethod
{
public:
    virtual ~InitializeMethod() {}

    virtual bool Initialize() = 0;
};

class IsInitializedMethod
{
public:
    virtual ~IsInitializedMethod() {}

    virtual bool IsInitialized() const = 0;
};

/**
 * @brief The InitTypeMethod class
 */
template <typename T, typename Return = bool>
class InitTypeMethod
{
public:
    virtual ~InitTypeMethod() {}

    virtual Return Init(T&) = 0;
};

/**
 * @brief The PassivateMethod class
 */
class PassivateMethod
{
public:
    virtual ~PassivateMethod() {}

    virtual bool Passivate() = 0;
};

class IsPassivatedMethod
{
public:
    virtual ~IsPassivatedMethod() {}

    virtual bool IsPassivated() = 0;
};

/**
 * @brief The ActivateMethod class
 */
class ActivateMethod
{
public:
    virtual ~ActivateMethod() {}

    virtual bool Activate() = 0;
};

class IsActivatedMethod
{
public:
    virtual ~IsActivatedMethod() {}

    virtual bool IsActivated() const = 0;
};


class DeactivateMethod
{
public:
    virtual ~DeactivateMethod() {}

    virtual bool Deactivate() = 0;
};

/**
* @brief The ActivateMethod class
*/
class ActiveMethod
{
public:
    virtual ~ActiveMethod() {}

    virtual void Active() = 0;
};

class IsActiveMethod
{
public:
    virtual ~IsActiveMethod() {}

    virtual bool IsActive() const = 0;
};

/**
 * @brief The ShutdownMethod class
 */
class ShutdownMethod
{
public:
    virtual ~ShutdownMethod() {}

    virtual bool Shutdown() = 0;
};

/**
 * @brief The FinalizeMethod class
 */
class FinalizeMethod
{
public:
    virtual ~FinalizeMethod() {}

    virtual bool Finalize() = 0;
};

class IsFinalizedMethod
{
public:
    virtual ~IsFinalizedMethod() {}

    virtual bool IsFinalized() const = 0;
};

template <typename Return>
class RefreshMethod
{
public:
    virtual ~RefreshMethod() {}

    virtual Return Refresh() = 0;
};

class IsRefreshedMethod
{
public:
    virtual ~IsRefreshedMethod() {}

    virtual bool IsRefreshed() const = 0;
};

template <typename Return>
class ReloadMethod
{
public:
    virtual ~ReloadMethod() {}

    virtual Return Reload() = 0;
};

class IsReloadedMethod
{
public:
    virtual ~IsReloadedMethod() {}

    virtual bool IsReloaded() const = 0;
};

/**
 * @brief The DisposeMethod class
 *
 * TODO: Alternative name: Destroy(), Delete(), Invalidate()
 */
class DisposeMethod
{
public:
    virtual bool Dispose() = 0;
};

class IsDisposedMethod
{
public:
    virtual bool IsDisposed() const = 0;
};

//template <typename T>
//class ObjectLifecycle : public FactoryMethod<T>
//                      , public InitMethod
//                      , public DisposeMethod
//                      , public PassivateMethod
//                      , public ActivateMethod
//                      , public FinalizeMethod
//{
//};


class InitializeMethods
    : public InitializeMethod
      , public IsInitializedMethod
{
public:
    virtual ~InitializeMethods() {}
};

class FinalizeMethods
    : public FinalizeMethod
      , public IsFinalizedMethod
{
public:
    virtual ~FinalizeMethods() {}
};

class ActivationMethods
    : public Templates::IsActiveMethod
      , public Templates::ActivateMethod
      , public Templates::DeactivateMethod
{
public:
    virtual ~ActivationMethods()
    { }
};

class ObjectLifecycle : public InitializeMethods
                      , public ActivationMethods
                      , public FinalizeMethods
                      //, public DisposeMethod
{ };

class Lifecycle : public ObjectLifecycle
{
public:
    virtual ~Lifecycle()
    { }
};

}}

#endif
