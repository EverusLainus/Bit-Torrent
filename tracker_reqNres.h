#include <string>
#include <iostream>
#include <curl/curl.h>
#include <assert.h>
#include <libtorrent/bdecode.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/entry.hpp>

struct AnnounceRequest {
    std::string trackerUrl;
    std::string info_hash;
    std::string peer_id;
    int port;
    long uploaded;
    long downloaded;
    long left;
};

enum event_state{
   EMPTY, STARTED, STOPPED, COMPLETED
};


