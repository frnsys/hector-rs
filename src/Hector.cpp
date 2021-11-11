/*
 * Copyright (c) 2017 Sven Willner <sven.willner@pik-potsdam.de>
 * Free software under GNU Affero General Public License v3, see LICENSE
 */

#include "Hector.h"
#include <stdexcept>
#include "h_util.hpp"
#include "component_data.hpp"
#include "message_data.hpp"
#include "unitval.hpp"
#include "h_exception.hpp"

namespace hector = Hector;

namespace rust_hector {

std::string Hector::version() {
    return MODEL_VERSION;
}

bool Hector::Visitor::shouldVisit(bool in_spinup, double date) {
    current_date = date;
    return true;
}

void Hector::Visitor::visit(hector::Core* hcore) {
    const auto time_index = static_cast<int>(current_date - hcore->getStartDate() - 1);
    for (auto& observable : observables) {
        observable.read_data(hcore, current_date, time_index, spinup_size);
    }
    if (hcore->inSpinup()) {
        ++spinup_size;
    }
}

hector::Core* Hector::core() {
    if (!core_) {
        reset();
    }
    return core_.get();
}

void Hector::add_observable(std::string component, std::string name, bool needs_date, bool in_spinup) {
    visitor.observables.emplace_back(Observable{core(), std::move(component), std::move(name), needs_date, in_spinup, run_size()});
}

std::vector<double> Hector::get_observable(const std::string component, const std::string name, const bool in_spinup) const {
    for (const auto& observable : visitor.observables) {
        if (observable.matches(component, name, in_spinup)) {
            return observable.get_array();
        }
    }
    throw std::runtime_error("Observable not found");
}

void Hector::clear_observables() { visitor.observables.clear(); }

std::size_t Hector::run_size() { return static_cast<std::size_t>(std::max(0.0, core()->getEndDate() - core()->getStartDate())); }

std::size_t Hector::spinup_size() const { return visitor.spinup_size; }

double Hector::end_date() { return core()->getEndDate(); }

double Hector::start_date() { return core()->getStartDate(); }

void Hector::run() {
    visitor.spinup_size = 0;
    core()->prepareToRun();
    core()->run(-1);
}

void Hector::shutdown() {
    core()->shutDown();
    core_.reset();
}

void Hector::reset() {
    core_.reset(new hector::Core(hector::Logger::WARNING, false, false));
    core_->init();
    core_->addVisitor(&visitor);
    for (auto& observable : visitor.observables) {
        observable.reset(core_.get());
    }
}

void Hector::set_string(const std::string& section, const std::string& variable, const std::string& value) {
    hector::message_data data(value);
    auto bracket_open = std::find(variable.begin(), variable.end(), '[');
    if (bracket_open != variable.end()) {
        data.date = std::stod(std::string(bracket_open + 1, std::find(bracket_open, variable.end(), ']')));
        core()->setData(section, std::string(variable.begin(), bracket_open), data);
    } else {
        hector::message_data data(value);
        core()->setData(section, variable, data);
    }
}

void Hector::set_double(const std::string& section, const std::string& variable, double value) {
    hector::message_data data(hector::unitval(value, hector::U_UNDEFINED));
    core()->setData(section, variable, data);
}

void Hector::set_timed_double(const std::string& section, const std::string& variable, std::size_t year, double value) {
    hector::message_data data(hector::unitval(value, hector::U_UNDEFINED));
    data.date = year;
    core()->setData(section, variable, data);
}

void Hector::set_timed_doubles(const std::string& section, const std::string& variable, const std::size_t* years, const double* values, size_t size) {
    for (std::size_t i = 0; i < size; ++i) {
        hector::message_data data(hector::unitval(values[i], hector::U_UNDEFINED));
        data.date = years[i];
        core()->setData(section, variable, data);
    }
}

void Hector::set_timed_array(const std::string& section, const std::string& variable, const std::vector<std::size_t>& years, const std::vector<double>& values) {
    if (years.size() != values.size()) {
        throw std::runtime_error("years and values should be of equal size");
    }
    set_timed_doubles(section, variable, &years[0], &values[0], years.size());
}

void Hector::set_double_unit(const std::string& section, const std::string& variable, double value, const std::string& unit) {
    hector::message_data data(hector::unitval(value, hector::unitval::parseUnitsName(unit)));
    core()->setData(section, variable, data);
}

void Hector::set_timed_double_unit(const std::string& section, const std::string& variable, std::size_t year, double value, const std::string& unit) {
    hector::message_data data(hector::unitval(value, hector::unitval::parseUnitsName(unit)));
    data.date = year;
    core()->setData(section, variable, data);
}

void Hector::set_timed_doubles_unit(
    const std::string& section, const std::string& variable, const std::size_t* years, const double* values, size_t size, const std::string& unit) {
    for (std::size_t i = 0; i < size; ++i) {
        hector::message_data data(hector::unitval(values[i], hector::unitval::parseUnitsName(unit)));
        data.date = years[i];
        core()->setData(section, variable, data);
    }
}

void Hector::set_timed_array_unit(const std::string& section,
                 const std::string& variable,
                 const std::vector<std::size_t>& years,
                 const std::vector<double>& values,
                 const std::string& unit) {
    if (years.size() != values.size()) {
        throw std::runtime_error("years and values should be of equal size");
    }
    set_timed_doubles_unit(section, variable, &years[0], &values[0], years.size(), unit);
}

std::unique_ptr<HectorClient> new_hector() {
  return std::make_unique<HectorClient>();
}

HectorClient::HectorClient() : hector(new class Hector) {}

void HectorClient::add_observable(const rust::Str component, const rust::Str name, bool need_date, bool in_spinup) const {
    auto _component = std::string(component);
    auto _name = std::string(name);
    hector->add_observable(_component, _name, need_date, in_spinup);
}
rust::Vec<double> HectorClient::get_observable(const rust::Str component, const rust::Str name, bool in_spinup) const {
    auto _component = std::string(component);
    auto _name = std::string(name);
    auto arr = hector->get_observable(_component, _name, in_spinup);
    rust::Vec<double> result;
    result.reserve(arr.size());
    for (auto &v: arr) {
        result.push_back(v);
    }
    return result;
}
void HectorClient::run() const {
    hector->run();
}
void HectorClient::shutdown() const {
    hector->shutdown();
}
void HectorClient::set_timed_array(const rust::Str section, const rust::Str variable, const rust::Vec<std::size_t>& years, const rust::Vec<double>& values) const {
    auto _section = std::string(section);
    auto _variable = std::string(variable);

    std::vector<std::size_t> _years;
    for (auto &v: years) {
        _years.emplace_back(v);
    }
    std::vector<double> _values;
    for (auto &v: values) {
        _values.emplace_back(v);
    }

    hector->set_timed_array(_section, _variable, _years, _values);
}

void HectorClient::set_double(const rust::Str section, const rust::Str variable, double value) const {
    auto _section = std::string(section);
    auto _variable = std::string(variable);
    hector->set_double(_section, _variable, value);
}

void HectorClient::set_double_unit(const rust::Str section, const rust::Str variable, double value, const rust::Str unit) const {
    auto _section = std::string(section);
    auto _variable = std::string(variable);
    auto _unit = std::string(unit);
    hector->set_double_unit(_section, _variable, value, _unit);
}

void HectorClient::set_timed_double(const rust::Str section, const rust::Str variable, std::size_t year, double value) const {
    auto _section = std::string(section);
    auto _variable = std::string(variable);
    hector->set_timed_double(_section, _variable, year, value);
}

void HectorClient::set_timed_double_unit(const rust::Str section, const rust::Str variable, std::size_t year, double value, const rust::Str unit) const {
    auto _section = std::string(section);
    auto _variable = std::string(variable);
    auto _unit = std::string(unit);
    hector->set_timed_double_unit(_section, _variable, year, value, _unit);

}

void HectorClient::set_string(const rust::Str section, const rust::Str variable, const rust::Str value) const {
    auto _section = std::string(section);
    auto _variable = std::string(variable);
    auto _value = std::string(value);
    hector->set_string(_section, _variable, _value);
}

}  // namespace rust_hector


