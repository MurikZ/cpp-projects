// lib/adapters.h

#ifndef ADAPTERS_H
#define ADAPTERS_H

#include <utility>
#include <iterator>
#include <type_traits>
#include <algorithm>

// Вспомогательные метафункции
template<typename T>
struct IsPair : std::false_type {};

template<typename K, typename V>
struct IsPair<std::pair<K, V>> : std::true_type {};

template<typename T>
inline constexpr bool IsPairV = IsPair<T>::value;

// Оператор пайплайна
template<typename Rng, typename Adpt>
auto operator|(Rng&& rng, Adpt&& adpt) {
    return std::forward<Adpt>(adpt)(std::forward<Rng>(rng));
}

// Фильтрация
template<typename Rng, typename Predicate>
class FilterAdapter {
    Rng rng_;
    Predicate pred_;

public:
    FilterAdapter(Rng rng, Predicate pred)
        : rng_(std::forward<Rng>(rng)), pred_(std::move(pred)) {}

    class Iter {
        using BaseIter = decltype(std::begin(std::declval<Rng&>()));
        BaseIter cur_;
        BaseIter end_;
        Predicate pred_;

        void SkipInvalid() {
            while (cur_ != end_ && !pred_(*cur_)) ++cur_;
        }

    public:
        Iter(BaseIter begin, BaseIter end, Predicate pred)
            : cur_(begin), end_(end), pred_(std::move(pred)) {
            SkipInvalid();
        }

        auto operator*() const { return *cur_; }
        Iter& operator++() {
            ++cur_;
            SkipInvalid();
            return *this;
        }
        bool operator!=(const Iter& other) const {
            return cur_ != other.cur_;
        }
    };

    auto begin() const { return Iter(std::begin(rng_), std::end(rng_), pred_); }
    auto end() const { return Iter(std::end(rng_), std::end(rng_), pred_); }
};

struct Filter {
    template<typename Predicate>
    auto operator()(Predicate&& pred) const {
        return [pred = std::forward<Predicate>(pred)](auto&& rng) {
            return FilterAdapter<decltype(rng), std::decay_t<Predicate>>(
                std::forward<decltype(rng)>(rng),
                std::move(pred)
            );
        };
    }
};

inline Filter filter;

// Преобразование
template<typename Rng, typename Function>
class TransformAdapter {
    Rng rng_;
    Function func_;

public:
    TransformAdapter(Rng rng, Function func)
        : rng_(std::forward<Rng>(rng)), func_(std::move(func)) {}

    class Iter {
        using BaseIter = decltype(std::begin(std::declval<Rng&>()));
        BaseIter it_;
        Function func_;

    public:
        Iter(BaseIter it, Function func)
            : it_(it), func_(std::move(func)) {}

        auto operator*() const { return func_(*it_); }
        Iter& operator++() { ++it_; return *this; }
        bool operator!=(const Iter& other) const { return it_ != other.it_; }
    };

    auto begin() const { return Iter(std::begin(rng_), func_); }
    auto end() const { return Iter(std::end(rng_), func_); }
};

struct Transform {
    template<typename Function>
    auto operator()(Function&& func) const {
        return [func = std::forward<Function>(func)](auto&& rng) {
            return TransformAdapter<decltype(rng), std::decay_t<Function>>(
                std::forward<decltype(rng)>(rng),
                std::move(func)
            );
        };
    }
};

inline Transform transform;

// Взятие элементов
template<typename Rng>
class TakeAdapter {
    Rng rng_;
    size_t cnt_;

public:
    TakeAdapter(Rng rng, size_t cnt)
        : rng_(std::forward<Rng>(rng)), cnt_(cnt) {}

    class Iter {
        using BaseIter = decltype(std::begin(std::declval<Rng&>()));
        BaseIter it_;
        BaseIter end_;
        size_t remain_;

    public:
        Iter(BaseIter it, BaseIter end, size_t remain)
            : it_(it), end_(end), remain_(remain) {}

        auto operator*() const { return *it_; }
        Iter& operator++() {
            ++it_;
            if (remain_ > 0) --remain_;
            return *this;
        }
        bool operator!=(const Iter& other) const {
            return it_ != other.it_ && remain_ != other.remain_;
        }
    };

    auto begin() const { return Iter(std::begin(rng_), std::end(rng_), cnt_); }
    auto end() const { return Iter(std::end(rng_), std::end(rng_), 0); }
};

struct Take {
    auto operator()(size_t cnt) const {
        return [cnt](auto&& rng) {
            return TakeAdapter<decltype(rng)>(
                std::forward<decltype(rng)>(rng),
                cnt
            );
        };
    }
};

inline Take take;

// Пропуск элементов
template<typename Rng>
class DropAdapter {
    Rng rng_;
    size_t cnt_;

public:
    DropAdapter(Rng rng, size_t cnt)
        : rng_(std::forward<Rng>(rng)), cnt_(cnt) {}

    class Iter {
        using BaseIter = decltype(std::begin(std::declval<Rng&>()));
        BaseIter it_;
        BaseIter end_;

    public:
        Iter(BaseIter it, BaseIter end)
            : it_(it), end_(end) {}

        auto operator*() const { return *it_; }
        Iter& operator++() { ++it_; return *this; }
        bool operator!=(const Iter& other) const { return it_ != other.it_; }
    };

    auto begin() const {
        auto it = std::begin(rng_);
        auto end = std::end(rng_);
        size_t n = cnt_;
        while (n-- > 0 && it != end) ++it;
        return Iter(it, end);
    }

    auto end() const { return Iter(std::end(rng_), std::end(rng_)); }
};

struct Drop {
    auto operator()(size_t cnt) const {
        return [cnt](auto&& rng) {
            return DropAdapter<decltype(rng)>(
                std::forward<decltype(rng)>(rng),
                cnt
            );
        };
    }
};

inline Drop drop;

// Разворот
template<typename Rng>
class ReverseAdapter {
    Rng rng_;

    using BaseIter = decltype(std::begin(std::declval<Rng&>()));
    using IterCategory = typename std::iterator_traits<BaseIter>::iterator_category;

    static_assert(
        std::is_base_of_v<std::bidirectional_iterator_tag, IterCategory>,
        "Reverse requires bidirectional iterators"
    );

public:
    ReverseAdapter(Rng rng)
        : rng_(std::forward<Rng>(rng)) {}

    auto begin() const { return std::rbegin(rng_); }
    auto end() const { return std::rend(rng_); }
};

struct Reverse {
    auto operator()() const {
        return [](auto&& rng) {
            return ReverseAdapter<decltype(rng)>(
                std::forward<decltype(rng)>(rng)
            );
        };
    }
};

inline Reverse reverse;

// Ключи
template<typename Rng>
class KeysAdapter {
    Rng rng_;

    using ValueType = typename std::decay_t<decltype(*std::begin(rng_))>;
    static_assert(IsPairV<ValueType>, "Keys requires a range of pairs");

public:
    KeysAdapter(Rng rng)
        : rng_(std::forward<Rng>(rng)) {}

    class Iter {
        using BaseIter = decltype(std::begin(std::declval<Rng&>()));
        BaseIter it_;

    public:
        Iter(BaseIter it) : it_(it) {}

        auto operator*() const { return it_->first; }
        Iter& operator++() { ++it_; return *this; }
        bool operator!=(const Iter& other) const { return it_ != other.it_; }
    };

    auto begin() const { return Iter(std::begin(rng_)); }
    auto end() const { return Iter(std::end(rng_)); }
};

struct Keys {
    auto operator()() const {
        return [](auto&& rng) {
            return KeysAdapter<decltype(rng)>(
                std::forward<decltype(rng)>(rng)
            );
        };
    }
};

inline Keys keys;

// Значения
template<typename Rng>
class ValuesAdapter {
    Rng rng_;

    using ValueType = typename std::decay_t<decltype(*std::begin(rng_))>;
    static_assert(IsPairV<ValueType>, "Values requires a range of pairs");

public:
    ValuesAdapter(Rng rng)
        : rng_(std::forward<Rng>(rng)) {}

    class Iter {
        using BaseIter = decltype(std::begin(std::declval<Rng&>()));
        BaseIter it_;

    public:
        Iter(BaseIter it) : it_(it) {}

        decltype(auto) operator*() const { return it_->second; }
        Iter& operator++() { ++it_; return *this; }
        bool operator!=(const Iter& other) const { return it_ != other.it_; }
    };

    auto begin() const { return Iter(std::begin(rng_)); }
    auto end() const { return Iter(std::end(rng_)); }
};

struct Values {
    auto operator()() const {
        return [](auto&& rng) {
            return ValuesAdapter<decltype(rng)>(
                std::forward<decltype(rng)>(rng)
            );
        };
    }
};

inline Values values;

#endif // ADAPTERS_H
