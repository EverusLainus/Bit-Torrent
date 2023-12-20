// request from tracker and give response.

#include "tracker_reqNres.h"

// Callback function to receive data from cURL
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);

    //parse the bencoded response
    libtorrent::entry response;
    libtorrent::error_code ec;
    libtorrent::bdecode((span<const char>)output->c_str(), output->c_str()+output->size(), (bdecode_node &)response, ec);

    if(ec){
        std::cerr << "Error parsing tracker response: " << ec.message() << std::endl;
        return 1;
    }


    // Access values based on keys
    int interval = response.dict_find_int_value("interval", 0);
    int complete = response.dict_find_int_value("complete", 0);
    int incomplete = response.dict_find_int_value("incomplete", 0);

    std::cout << "Interval: " << interval << " seconds\n";
    std::cout << "Complete: " << complete << " seeders\n";
    std::cout << "Incomplete: " << incomplete << " leechers\n";

    // Access the list of peers
    const libtorrent::entry* peers = response.dict_find_list("peers");
    if (peers) {
        for (auto& peer : peers->list()) {
            std::string ip = peer.dict_find_string_value("ip", "");
            int port = peer.dict_find_int_value("port", 0);

            std::cout << "Peer: " << ip << ":" << port << std::endl;
        }
    }

    return 0;
}

// Function to encode the AnnounceRequest into a URL for an HTTP GET request
void encodeAnnounceRequest(const AnnounceRequest& request) {
    //construct a full URL with parameters
    std::string fullUrl = request.trackerUrl+
                        "?info_hash="+ request.info_hash+
                        "&peer_id="+ request.peer_id+
                        "&port="+ std::to_string(request.port)+
                        "&uploaded="+std::to_string(request.uploaded)+
                        "&downloaded="+std::to_string(request.downloaded)+
                        "&left="+std::to_string(request.left);
    //initalist cURL
    CURL *curl = curl_easy_init();
    assert(curl !=NULL);

    //make a request to the given URL
     curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());

     //string to receive data
    std::string responseData;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

    // Perform the HTTP GET request
    CURLcode res = curl_easy_perform(curl);

    // Check for errors
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    } else {
        // Print the received data (replace with your own handling logic)
        std::cout << "Response: " << responseData << std::endl;
    }

    // Clean up
    curl_easy_cleanup(curl);   
}



int main() {
    // Example usage
    AnnounceRequest request;
    request.trackerUrl = "https://httpbin.org/";
    request.info_hash = "your_info_hash";
    request.peer_id = "your_peer_id";
    request.port = 80;  // Example port
    request.uploaded = 0;  // Example uploaded amount
    request.downloaded = 0;  // Example downloaded amount
    request.left = 100;  // Example amount left to download

    // Encode the request for an HTTP GET request
    encodeAnnounceRequest(request);
    
    return 0;
}

