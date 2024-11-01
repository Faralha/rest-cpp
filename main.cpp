#include <crow.h>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/options/find.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>

int main()
{
    mongocxx::instance instance{}; // This should be done only once.
    mongocxx::client client{mongocxx::uri{"mongodb://user-g:g-for-goodluck@db.nafkhanzam.com/pweb-g"}};
    auto db = client["testdb"];
    auto collection = db["links"];
  
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        return "Hello world!";
    });

    // GET route for sum calculation
    CROW_ROUTE(app, "/sum/<int>/<int>")([](int a, int b){
        int sum = a + b;
        crow::json::wvalue result;
        result["sum"] = sum;
        return crow::response(result);
    });

    // GET route to retrieve top 10 links sorted by clicks
    CROW_ROUTE(app, "/top10")([&collection](){
        mongocxx::options::find opts;
        opts.sort(bsoncxx::builder::stream::document{} << "clicks" << -1 << bsoncxx::builder::stream::finalize).limit(10);

        auto cursor = collection.find({}, opts);
        crow::json::wvalue result;
        crow::json::wvalue::list links;

        for (auto&& doc : cursor) {
            crow::json::wvalue link;
            link["slug"] = std::string(doc["slug"].get_string().value);
            link["link"] = std::string(doc["link"].get_string().value);
            link["clicks"] = doc["clicks"].get_int32().value;
            links.push_back(std::move(link));
        }

        result["links"] = std::move(links);
        return crow::response(result);
    });

    app.port(8000).multithreaded().run();
}