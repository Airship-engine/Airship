#include <map>
#include <optional>
#include <string>

/*
enum class ConVarType {
    String,
    Int,
    Float,
    Bool
};

enum class ConVar {
    Default_UserName,
    Render_BufferDepth
};

struct ConVarObject {
    ConVar var;
    ConVarType type;
    std::string defaultValue;
    std::string helpText;
};

ConVarObject conVars[] = {
    {ConVar::Default_UserName, ConVarType::String, "DefaultUser", "Defines the default name for a user when a platform is not available"},
    {ConVar::Render_BufferDepth, ConVarType::Int, "2", "Tells the rendering engine how many buffers to use"}
};
*/

enum class ConvarType {
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

protected:
    ConvarType m_Type;
};

template<typename value_type>
class Convar : public ConvarValue
{
public:
    Convar(value_type value) : ConvarValue(ConvarTypeTraits<value_type>::type), m_Value(value) {}

    value_type& get() { return m_Value; }

private:
    value_type m_Value;
};

class ConvarRegistry 
{
public: 
    static ConvarRegistry& get() {
        static ConvarRegistry singleton;
        return singleton;
    }

    //template<typename T>
    //T Read(const ConvarKey<T>& keyName) const {
        //return T();
    //}

    template<typename T, std::enable_if_t<!std::is_same_v<const char *, T>>>
    Convar<T> RegisterKey(std::string name, T value) {
        //ConvarKey{name, value};
        //m_ConvarMap[key.m_Name] = key;
    }

    template<typename T = const char*>
    Convar<std::string> RegisterKey(std::string name, T value) {
        return RegisterKey(name, std::string(value));
    }


    // User.DefaultName Some Default Name
    // Read("User.DefaultName")
    template<typename T>
    std::optional<Convar<T>> Read(std::string name)
    {
        if(m_ConvarMap.find(name) == m_ConvarMap.end())
            return std::nullopt;

        ConvarValue* value = m_ConvarMap.at(name);
        ConvarType varType = value->m_Type;
        ConvarType toType = ConvarTypeTraits<T>::type;
        if(varType != toType)
            return std::nullopt;

        return dynamic_cast<Convar<T>>(value);
    }

private:
    std::map<std::string, ConvarValue*> m_ConvarMap;
};

namespace CV 
{
    //constexpr const char* g_DefaultUserName = "User.DefaultName";
    //const ConvarKey<std::string> g_DefaultUserName{"User.DefaultName", "DefaultName"};
    //const ConvarKey<const char*> g_DefaultCharValue{"User.DefaultName", "DefaultName"};

}

#define CONVAR(name, value) Convars::get().RegisterKey(name, value)
#define CONVAR_STR(name, value) CONVAR(name, std::string(value))

int main() 
{
    const Convar<std::string>& g_NewConvar = ConvarRegistry::get().RegisterKey("User.DefaultName", "DefaultName");

    //CONVAR("User.DefaultName", "DefaultName");
    //CONVAR("User.DefaultLevel", 3);
//
    //ConvarKey<std::string> defaultUsername = ConvarRegistry::get().RegisterKey("User.DefaultName", "DefaultName");
    //ConvarRegistry::get().RegisterKey("Render.BufferDepth", 2);

    //Convars::get().Read(ConvarStore::DefaultName)
}

//CONVAR_STRING(g_ConVar_DefaultUserName, "DefaultUser", "Defines the default name for a user when a platform is not available")

//std::string i = ConvarRegistry::get().Read(CV::g_DefaultUserName);

//BEGIN_CONVAR_MAP()
    //CONVAR_STRING("User.DefaultName", "DefaultUser", "Defines the default name for a user when a platform is not available")
    //CONVAR_INT("Render.BufferDepth", 2, "Tells the rendering engine how many buffers to use")
//
    //CONVAR_EXEC("User.DefaultName", &RunSomeFunction, "Defines the default name for a user when a platform is not available")
//END_CONVAR_MAP()
//