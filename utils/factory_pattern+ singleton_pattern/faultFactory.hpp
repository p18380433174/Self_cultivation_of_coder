#ifndef __FAULT_FACTORY_HPP__
#define __FAULT_FACTORY_HPP__

#include <map>
#include <string>

template<typename FaultType_t>
class IFaultRegistrar
{
public:
    virtual FaultType_t* createImplement() = 0;
protected:
    virtual ~IFaultRegistrar() {};
};

template <typename FaultType_t>
class FaultFactory
{
public:
    // 获取工厂单例,工厂的实例是唯一的
    static FaultFactory<FaultType_t>& instance()
    {
        static FaultFactory<FaultType_t> instance;
        return instance;
    }

    // 产品注册
    void registerFault(std::string name, IFaultRegistrar<FaultType_t>* registrar)
    {
        m_FaultRegistry[name] = registrar;
    }

    // 根据名字name，获取对应具体的对象
    FaultType_t* getFaultImplement(std::string name)
    {
        auto iter = m_FaultRegistry.find(name);
        if (iter == m_FaultRegistry.end())
        {
            std::cout << "No fault register found\n";
            return nullptr;
        }
        return iter->second->createImplement();
    }

private:
    // 禁止外部构造和析构
    FaultFactory() {}
    ~FaultFactory() {}

    // 禁止拷贝和赋值操作
    FaultFactory(const FaultFactory&);
    const FaultFactory &operator=(const FaultFactory&);

    // 保存注册过的产品,key - 产品名字 value - 产品类型
    std::map<std::string, IFaultRegistrar<FaultType_t>*> m_FaultRegistry;
};

template<typename FaultType_t, typename FaultImpl_t>
class FaultRegistrar : public IFaultRegistrar<FaultType_t>
{
public:
    // 构造函数,注册产品到工厂,只能显式调用
    static FaultRegistrar<FaultType_t, FaultImpl_t>& instance()
    {
        static FaultRegistrar implement;
        return implement;
    }
    
    void reigsterFault(std::string name)
    {
        FaultFactory<FaultType_t>::instance().registerFault(name, this);
    }

    // 创建具体产品指针
    FaultType_t* createImplement() override
    {
        static FaultType_t* implement = new FaultImpl_t;

        return implement;
    }
private:
    FaultRegistrar() = default;
};

#endif