#include "rust/cxx.h"
#include <memory>
#include <string>
#include <vector>
#include "Observable.h"
#include "core.hpp"
#include "avisitor.hpp"

namespace hector = Hector;

namespace rust_hector {

class Hector {
  private:
    std::unique_ptr<hector::Core> core_;

  protected:
    class Visitor : hector::AVisitor {
        friend class Hector;

      protected:
        double current_date = 0;
        std::vector<Observable> observables;
        std::size_t spinup_size = 0;

      public:
        bool shouldVisit(const bool in_spinup, const double date);
        void visit(hector::Core* core);
    };

    Visitor visitor;
    inline hector::Core* core();

  public:
    static std::string version();
    void add_observable(std::string component, std::string name, bool need_date, bool in_spinup);
    std::vector<double> get_observable(const std::string component, const std::string name, bool in_spinup) const;
    void clear_observables();
    std::size_t run_size();
    std::size_t spinup_size() const;
    double end_date();
    double start_date();
    void run();
    void shutdown();
    void reset();
    void set_string(const std::string& section, const std::string& variable, const std::string& value);
    void set_double(const std::string& section, const std::string& variable, double value);
    void set_timed_double(const std::string& section, const std::string& variable, std::size_t year, double value);
    void set_timed_doubles(const std::string& section, const std::string& variable, const std::size_t* years, const double* values, size_t size);
    void set_timed_array(const std::string& section, const std::string& variable, const std::vector<std::size_t>& years, const std::vector<double>& values);
    void set_double_unit(const std::string& section, const std::string& variable, double value, const std::string& unit);
    void set_timed_double_unit(const std::string& section, const std::string& variable, std::size_t year, double value, const std::string& unit);
    void set_timed_doubles_unit(const std::string& section, const std::string& variable, const std::size_t* years, const double* values, size_t size, const std::string& unit);
    void set_timed_array_unit(const std::string& section,
             const std::string& variable,
             const std::vector<std::size_t>& years,
             const std::vector<double>& values,
             const std::string& unit);
};

class HectorClient {
    private:
        std::shared_ptr<Hector> hector;

    public:
        HectorClient();
        void add_observable(const rust::Str component, const rust::Str name, bool need_date, bool in_spinup) const;
        rust::Vec<double> get_observable(const rust::Str component, const rust::Str name, bool in_spinup) const;
        void run() const;
        void shutdown() const;
        void set_timed_array(const rust::Str section, const rust::Str variable, const rust::Vec<std::size_t>& years, const rust::Vec<double>& values) const;
        void set_double(const rust::Str section, const rust::Str variable, double value) const;
        void set_timed_double(const rust::Str section, const rust::Str variable, std::size_t year, double value) const;
        void set_double_unit(const rust::Str section, const rust::Str variable, double value, const rust::Str unit) const;
        void set_timed_double_unit(const rust::Str section, const rust::Str variable, std::size_t year, double value, const rust::Str unit) const;
        void set_string(const rust::Str section, const rust::Str variable, const rust::Str value) const;
};

std::unique_ptr<HectorClient> new_hector();

}  // namespace rust_hector

