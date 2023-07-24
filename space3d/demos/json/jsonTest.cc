
#if defined(__GNUC__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#    pragma warning(disable : 4996)
#endif
#include <iostream>
#include <string>
#include <json/include/json.h>

void test01()
{

    std::cout << "jsonTest proj init !!!\n";
    //const char* jsonStr = "{\"no\": 666, \"name\": \"jack\",\"age\": 18,\"sex\": \"man\",\"like\":\"fruits\"}";
    std::string  jsonStr = R"({
    "number":   78,
    "name":     "jack",
    "age":      18,
    "sex":      "man",
    "like":     "eating fruits"
})";
    Json::Reader reader;
    Json::Value  root;
    if (reader.parse(jsonStr, root))
    {
        std::string name = root["name"].asString();
        int         no   = root["no"].asInt();
        int         age  = root["age"].asInt();
        std::string sex  = root["sex"].asString();
        std::string like = root["like"].asString();
        std::cout << "no:" << no << ", "
                  << "name:" << name << ", "
                  << "age:" << age << ", "
                  << "sex:" << sex << ", ";
        std::cout << "like: " << like << std::endl;
    }
    else
    {
        std::cout << "json parse error !!!" << std::endl;
    }
}
void test02()
{
    auto        taskID         = 1001;
    auto        progress       = 50;
    auto        renderingTimes = 5;
    std::string statusJson     = R"({
"rendering-ins":"jetty-scene-renderer",
"rendering-task":
    {
        "uuid":"rtrt88970-8990",
        "taskID":)" +
        std::to_string(taskID) + R"(,
        "name":"high-image-rendering",
        "phase":"finish",
        "progress":)" +
        std::to_string(progress) + R"(,
        "times":)" +
        std::to_string(renderingTimes) + R"(
    },
"rendering-status":"task:running"
})";
    Json::Reader reader;
    Json::Value  root;
    if (reader.parse(statusJson, root))
    {
        std::string renderingIns = root["rendering-ins"].asString();
        std::cout << "renderingIns: " << renderingIns << std::endl;

        bool isObject = root["rendering-task"].isObject();
        std::cout << "isObject: " << isObject << std::endl;
        if (isObject)
        {
            Json::Value renderingTask = root["rendering-task"];
            std::string task_uuid     = renderingTask["uuid"].asString();
            std::cout << "    >>> task_uuid: " << task_uuid << std::endl;
        }
    }
    else
    {
        std::cout << "json parse error !!!" << std::endl;
    }
}
int main()
{
    //test01();
    test02();
    return EXIT_SUCCESS;
}