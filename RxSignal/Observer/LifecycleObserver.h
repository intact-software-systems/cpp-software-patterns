#pragma once

namespace BaseLib { namespace Observer
{
/**
 * @brief The FactoryObserver class
 */
template <typename T>
class FactoryObserver
{
public:
    virtual void OnCreate(T) = 0;
};

/**
 * @brief The InitObserver class
 */
class InitObserver
{
public:
    virtual void OnInit() = 0;
};

/**
 * @brief The InitTypeObserver class
 */
template <typename T>
class OneArgInitObserver
{
public:
    virtual void OnInit(T) = 0;
};

/**
 * @brief The PassivateObserver class
 */
class PassivateObserver
{
public:
    virtual void OnPassivate() = 0;
};

/**
 * @brief The ActivateObserver class
 */
class ActivateObserver
{
public:
    virtual void OnActivate() = 0;
};

/**
 * @brief The FinalizeObserver class
 */
class FinalizeObserver
{
public:
    virtual void OnFinalize() = 0;
};

/**
 * @brief The DisposeObserver class
 *
 * TODO: Alternative name: Destroy()
 */
class DisposeObserver
{
public:
    virtual void OnDispose() = 0;
};


template <typename T>
class TypeLifecycleObserver
        : public FactoryObserver<T>
        , public InitObserver
        , public DisposeObserver
        , public PassivateObserver
        , public ActivateObserver
        , public FinalizeObserver
{ };

/**
 * @brief The ObjectLifecycle class
 */
class ObjectLifecycleObserver
        : public InitObserver
        , public DisposeObserver
        , public PassivateObserver
        , public ActivateObserver
        , public FinalizeObserver
{ };

}}
