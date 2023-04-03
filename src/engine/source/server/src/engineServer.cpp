/* Copyright (C) 2015-2022, Wazuh Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */

#include "server/engineServer.hpp"

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <logging/logging.hpp>

#include "endpoints/apiEndpoint.hpp"
#include "endpoints/eventEndpoint.hpp"
#include <metrics/metricsManager.hpp>

using moodycamel::BlockingConcurrentQueue;

using namespace engineserver::endpoints;

namespace engineserver
{

EngineServer::EngineServer(const std::string& apiEndpointPath,
                           std::shared_ptr<api::Registry> registry,
                           const std::string& eventEndpointPath,
                           std::optional<std::string> pathFloodedFile,
                           const int bufferSize,
                           const std::shared_ptr<metricsManager::IMetricsManager>& metricsManager)
{
    try
    {
        m_spMetricsScopeAPI = metricsManager->getMetricsScope("API");
        m_spMetricsScope = metricsManager->getMetricsScope("Server");
        m_spMetricsScopeDelta = metricsManager->getMetricsScope("ServerRate", true);
        if (nullptr == registry)
        {
            registry = std::make_shared<api::Registry>();
        }
        auto apiEndpoint = std::make_shared<APIEndpoint>(apiEndpointPath, registry, m_spMetricsScopeAPI);
        apiEndpoint->configure();
        m_endpoints.emplace(EndpointType::API, std::move(apiEndpoint));

        auto eventQueue = std::make_shared<BlockingConcurrentQueue<base::Event>>(bufferSize);
        auto eventEndpoint = std::make_shared<EventEndpoint>(eventEndpointPath, eventQueue, m_spMetricsScope, m_spMetricsScopeDelta, pathFloodedFile);
        eventEndpoint->configure();
        m_endpoints.emplace(EndpointType::EVENT, std::move(eventEndpoint));
    }
    catch (const std::exception& e)
    {
        WAZUH_LOG_ERROR(
            "Engine server: An exception ocurred while building the server: {}",
            e.what());
        throw;
    }
}

void EngineServer::run(void)
{
    if (m_endpoints.size() > 0)
    {
        WAZUH_LOG_INFO("Engine server: Starting...");
        m_endpoints.begin()->second->run();
    }
    else
    {
        WAZUH_LOG_WARN("Engine server: The server cannot be started, there are no "
                       "endpoints configured.");
    }
}

void EngineServer::close(void)
{
    if (0 < m_endpoints.size())
    {
        m_endpoints.begin()->second->close(); // TODO: All or only the first one?
    }
}

std::shared_ptr<moodycamel::BlockingConcurrentQueue<base::Event>> EngineServer::getEventQueue() const
{
    if (m_endpoints.find(EndpointType::EVENT) != m_endpoints.end())
    {
        // cast and get queue
        const auto& baseEndpoint = m_endpoints.at(EndpointType::EVENT);
        return std::static_pointer_cast<EventEndpoint>(baseEndpoint)->getEventQueue();
    }
    return nullptr;
}

std::shared_ptr<api::Registry> EngineServer::getRegistry() const
{
    if (m_endpoints.find(EndpointType::API) != m_endpoints.end())
    {
        // cast and get registry
        const auto& baseEndpoint = m_endpoints.at(EndpointType::API);
        const auto apiEndpoint = std::static_pointer_cast<APIEndpoint>(baseEndpoint);
        return apiEndpoint->getRegistry();
    }
    return nullptr;
}

} // namespace engineserver
