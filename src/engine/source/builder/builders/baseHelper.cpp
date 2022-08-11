/* Copyright (C) 2015-2021, Wazuh Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */

#include "baseHelper.hpp"

#include <algorithm>
#include <optional>
#include <sstream>
#include <variant>

#include <fmt/format.h>

#include "baseTypes.hpp"
#include "syntax.hpp"

namespace helper::base
{

std::tuple<std::string, std::string, std::vector<std::string>>
extractDefinition(const std::any& definition)
{
    std::tuple<std::string, std::string, std::vector<std::string>> extracted;
    try
    {
        extracted =
            std::any_cast<std::tuple<std::string, std::string, std::vector<std::string>>>(
                definition);
    }
    catch (const std::bad_any_cast& e)
    {
        std::throw_with_nested(std::runtime_error(
            "[builders::processDefinition(definition)] "
            "Can not process definition, expected tuple with name and parameters"));
    }

    return extracted;
}

std::vector<Parameter> processParameters(const std::vector<std::string>& parameters)
{
    std::vector<Parameter> newParameters;
    std::transform(parameters.begin(),
                   parameters.end(),
                   std::back_inserter(newParameters),
                   [](const std::string& parameter) -> Parameter
                   {
                       if (builder::internals::syntax::REFERENCE_ANCHOR == parameter[0])
                       {
                           std::string pointerPath;
                           try
                           {
                               pointerPath =
                                   json::Json::formatJsonPath(parameter.substr(1));
                           }
                           catch (const std::exception& e)
                           {
                               std::throw_with_nested(std::runtime_error(fmt::format(
                                   "[builders::processParameters(parameters)] "
                                   "Can not format to Json pointer path parameter: {}",
                                   parameter)));
                           }
                           return {Parameter::Type::REFERENCE, pointerPath};
                       }
                       else
                       {
                           return {Parameter::Type::VALUE, parameter};
                       }
                   });

    return newParameters;
}

void checkParametersSize(const std::vector<Parameter>& parameters, size_t size)
{
    if (parameters.size() != size)
    {
        throw std::runtime_error(fmt::format("[builders::assertParametersSize] "
                                             "Expected [{}] parameters, got [{}]",
                                             size,
                                             parameters.size()));
    }
}

void checkParametersMinSize(const std::vector<Parameter>& parameters, size_t minSize)
{
    if (parameters.size() < minSize)
    {
        throw std::runtime_error(fmt::format("[builders::assertParametersSize] "
                                             "Expected at least [{}] parameters, got [{}]",
                                             minSize,
                                             parameters.size()));
    }
}

void checkParameterType(const Parameter& parameter, Parameter::Type type)
{
    if (parameter.m_type != type)
    {
        throw std::runtime_error(fmt::format(
            "[builders::assertParameterType] "
            "Expected parameter of type [{}], got parameter [{}] with type [{}]",
            static_cast<int>(type),
            parameter.m_value,
            static_cast<int>(parameter.m_type)));
    }
}

std::string formatHelperFilterName(const std::string& name,
                                   const std::string& targetField,
                                   const std::vector<Parameter>& parameters)
{
    std::stringstream formattedName;
    formattedName << fmt::format("helper.{}[{}", name, targetField);
    if (parameters.size() > 0)
    {
        formattedName << fmt::format(", {}", parameters.begin()->m_value);
        for (auto it = parameters.begin() + 1; it != parameters.end(); ++it)
        {
            formattedName << fmt::format(", {}", it->m_value);
        }
    }
    formattedName << "]";

    return formattedName.str();
}
} // namespace helper::base
