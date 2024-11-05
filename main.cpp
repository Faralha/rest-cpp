#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/options/find.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <crow.h>
#include <iostream>

int main()
{
    mongocxx::instance instance{}; // This should be done only once.
    mongocxx::client client{mongocxx::uri{"mongodb://user-g:g-for-goodluck@db.nafkhanzam.com/pweb-g"}};
    auto db = client["pweb-g"];
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

    // GET route to retrieve top 10 links sorted by clicks and return to client
    CROW_ROUTE(app, "/top10")([&collection](){
        crow::json::wvalue result;
        result["links"] = crow::json::wvalue::list();
        try {
            mongocxx::options::find opts;
            opts.limit(10);
            opts.sort(bsoncxx::builder::stream::document{} << "clicks" << -1 << bsoncxx::builder::stream::finalize);

            auto cursor = collection.find({}, opts);

            for (auto&& doc : cursor) {
                if (doc["link"] && doc["link"].type() == bsoncxx::type::k_utf8) {
                    std::string link = std::string(doc["link"].get_utf8().value.data(), doc["link"].get_utf8().value.size());
                    int clicks = doc["clicks"].get_int32();
                    crow::json::wvalue link_data;
                    link_data["link"] = link;
                    link_data["clicks"] = clicks;
                    result["links"].push_back(std::move(link_data));
                } else {
                    std::cerr << "Document missing 'link' field or 'link' field is not a string." << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "An error occurred: " << e.what() << std::endl;
            return crow::response(500, "Internal Server Error");
        }
        return crow::response(result);
    });

    app.port(8000).multithreaded().run();

    return EXIT_SUCCESS;
}