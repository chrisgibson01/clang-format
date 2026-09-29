// Style sample — exercises every rule in .clang-format. Not meant to compile cleanly.
#include "myproject/widget.h"
#include "gtest/gtest.h"
#include <algorithm>
#include <map>
#include <string>
#include <vector>

#define MULTILINE_MACRO(x)                                                               \
    do                                                                                   \
    {                                                                                    \
        frobnicate(x);                                                                   \
    } while(0)

extern "C"
{
    int c_entry_point(int argc, char** argv);
}

std::string build_description(const std::string& widget_name,
                              int widget_value,
                              std::size_t threshold_count,
                              bool strict_mode,
                              const char* verbosity);

namespace outer
{
    namespace inner
    {
        enum class Colour
        {
            red,
            green,
            blue
        };

        union Raw
        {
            int as_int;
            float as_float;
        };

        template<typename T>
        concept Numeric = std::is_arithmetic_v<T>;

        class Base
        {
        public:
            virtual ~Base() = default;
            virtual int value() const = 0;
        };

        struct Tag
        {
        };

        class Widget final : public Base, private Tag
        {
        public:
            Widget(int identifier, std::string name, std::vector<int> samples):
                identifier_{identifier},
                name_{std::move(name)},
                samples_{std::move(samples)}
            {
            }

            int value() const override { return identifier_; }
            const std::string& name() const { return name_; }
            const std::vector<int>& samples() const { return samples_; }

            template<typename T>
                requires Numeric<T>
            T scaled(T factor) const
            {
                return static_cast<T>(identifier_) * factor;
            }

        private:
            int identifier_;
            std::string name_;
            std::vector<int> samples_;
        };

        int classify(const Widget& widget,
                     const std::map<std::string, int>& thresholds,
                     bool strict)
        {
            int* score = nullptr;
            const auto& name = widget.name();

            bool eligible = widget.value() > 10
                         && thresholds.size() < 100
                         && !strict
                         && name.size() > 3
                         && widget.samples().size() > 2;

            if(auto it = thresholds.find(name); it != thresholds.end())
            {
                score = const_cast<int*>(&it->second);
            }
            else if(strict)
            {
                return -1;
            }
            else
            {
                return 0;
            }

            auto description = build_description(widget.name(),
                                                 widget.value(),
                                                 thresholds.size(),
                                                 strict,
                                                 "verbose");

            switch(*score)
            {
            case 0:
                return widget.value();
            case 1:
            {
                auto scaled = widget.scaled(2.5);
                return scaled > 100.0 ? static_cast<int>(scaled) : widget.value();
            }
            default:
                break;
            }

            long total = 0;
            for(int i = 0; i < *score; ++i)
            {
                total += i * widget.value();
            }

            BOOST_FOREACH(int sample, widget.samples())
            {
                total += sample;
            }

            while(total > 0 && eligible)
            {
                total /= 2;
            }

            const char* verdict = total > static_cast<long>(thresholds.size())
                                    ? "widget exceeded the configured threshold value"
                                    : "widget remained within the configured threshold";

            try
            {
                auto results = std::vector<int>{1, 2, 3};
                std::sort(results.begin(),
                          results.end(),
                          [](int lhs, int rhs) { return lhs < rhs; });
            }
            catch(const std::exception& error)
            {
                return -2;
            }

            (void)description; // exercise trailing comment
            (void)verdict;     // alignment across lines
            return static_cast<int>(total);
        }
    }
}
