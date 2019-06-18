#include "thr.h"

#include "easylogging++.h"

#include <boost/program_options.hpp>

#include <iostream>
#include <fstream>

#include "c2c/p2p.h"
#include "chat.h"

INITIALIZE_EASYLOGGINGPP

using namespace std::placeholders;
namespace po = boost::program_options;

thr::thr(int argc, char *argv[])
{
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);

    po::options_description desc("Options");
    boost::program_options::options_description_easy_init opt = desc.add_options();

    c2c::p2p::server::set_options_desc(opt);
    chat::server::set_options_desc(opt);

    opt("config-file", po::value<std::string>(), "filepath of config");

    po::variables_map vm;

    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if(vm.count("config-file") > 0)
        {
            std::string fpath = vm["config-file"].as<std::string>();
            std::ifstream fconfig(fpath);
            po::store(po::parse_config_file<char>(fconfig , desc), vm);
        }

        p2p_ = c2c::p2p::server::create(ios_, vm);
        chat_ = std::make_shared<chat::server>(p2p_, vm);
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << e.what();
        return;
    }
}

void thr::start()
{
    try
    {
        p2p_->start();
        chat_->start();
        thr_ = std::thread([this]{ ios_.run(); });
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << e.what();
        return;
    }
}

void thr::stop()
{
    ios_.stop();
    chat_->stop();
    thr_.join();

    LOG(INFO) << "p2p service stopped";
}

std::shared_ptr<chat::server> thr::chat()
{
    return chat_;
}
