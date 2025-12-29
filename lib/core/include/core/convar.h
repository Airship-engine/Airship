#pragma once 

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>

namespace Airship
{

template< typename T>
using not_cstring = std::enable_if_t<!std::is_same_v<const char *, T>>;

enum class ConvarType : uint8_t {
    String,
    Float,
    Int, 
    Bool
};

template <typename T>
struct ConvarTypeTraits {};

template <>
struct ConvarTypeTraits<std::string>
{
    static const auto type = ConvarType::String;
};

template <>
struct ConvarTypeTraits<float>
{
    static const auto type = ConvarType::Float;
};

template <>
struct ConvarTypeTraits<int>
{
    static const auto type = ConvarType::Int;
};

template <>
struct ConvarTypeTraits<bool>
{
    static const auto type = ConvarType::Bool;
};

class ConvarValue 
{
public:
    ConvarValue(ConvarType type) : m_Type(type) {}
    virtual ~ConvarValue() = default;

    [[nodiscard]] ConvarType type() const { return m_Type; }

protected:
    ConvarType m_Type;
};

template<typename value_type>
class Convar : public ConvarValue
{
public:
    Convar(value_type value) : ConvarValue(ConvarTypeTraits<value_type>::type), m_Value(std::move(value)) {}

    value_type& get() { return m_Value; }

    Convar& operator=(value_type val)
    {
        m_Value = val;
        return *this;
    }

    auto operator<=>(value_type val) const
    {
        return m_Value <=> val;
    }

    bool operator==(value_type val) const
    {
        return m_Value == val;
    }

private:
    value_type m_Value;
};

class ConvarRegistry 
{
public: 

    template<typename T, typename = not_cstring<T>>
    Convar<T>* RegisterKey(const std::string& name, const T& value) {
        if (m_ConvarMap.contains(name))
        {
            // TODO: inform when m_ConvarMap already contains name
            ConvarValue* val = m_ConvarMap.at(name).get();
            return dynamic_cast<Convar<T>*>(val);
        }

        auto it = m_ConvarMap.insert({name, std::make_unique<Convar<T>>(value)}).first;
        return dynamic_cast<Convar<T>*>(it->second.get());
    }
    Convar<std::string>* RegisterKey(const std::string& name, const char* value) {
        return RegisterKey(name, std::string(value));
    }

    template<typename T>
    std::optional<Convar<T>*> read(const std::string& name)
    {
        if (!m_ConvarMap.contains(name))
        {
            // TODO: Warn of a missing key
            return std::nullopt;
        }

        ConvarValue* value = m_ConvarMap.at(name).get();
        const ConvarType varType = value->type();
        const ConvarType toType = ConvarTypeTraits<T>::type;
        if(varType != toType)
        {
            // TODO: Warn of an invalid conversion
            return std::nullopt;
        }

        return dynamic_cast<Convar<T>*>(value);
    }

    [[nodiscard]] size_t size() const { return m_ConvarMap.size(); }

private:
    std::map<std::string, std::unique_ptr<ConvarValue>> m_ConvarMap;
};

} // namespace Airship
