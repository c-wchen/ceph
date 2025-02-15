// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#ifndef CEPH_CACHE_CACHE_SERVER_H
#define CEPH_CACHE_CACHE_SERVER_H

#include <boost/asio.hpp>
#include <boost/asio/error.hpp>

#include "Types.h"
#include "SocketCommon.h"
#include "CacheSession.h"


using boost::asio::local::stream_protocol;

namespace ceph
{
namespace immutable_obj_cache
{

class CacheServer
{
public:
    CacheServer(CephContext *cct, const std::string &file, ProcessMsg processmsg);
    ~CacheServer();

    int run();
    int start_accept();
    int stop();

private:
    void accept();
    void handle_accept(CacheSessionPtr new_session,
                       const boost::system::error_code &error);

private:
    CephContext *cct;
    boost::asio::io_service m_io_service;
    ProcessMsg m_server_process_msg;
    stream_protocol::endpoint m_local_path;
    stream_protocol::acceptor m_acceptor;
};

}  // namespace immutable_obj_cache
}  // namespace ceph

#endif
