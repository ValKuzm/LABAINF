#pragma once
#ifndef LAZYSEQUENCE_H
#define LAZYSEQUENCE_H

#include <map>
#include <vector>
#include <functional>
#include <stdexcept>
#include <string>

// -----------------------------
//  Exceptions (unique names)
// -----------------------------
class LSQ_IndexOutOfRange : public std::out_of_range {
public:
    LSQ_IndexOutOfRange() : std::out_of_range("Index out of range") {}
    LSQ_IndexOutOfRange(const std::string& msg) : std::out_of_range(msg) {}
};

class LSQ_EndOfStream : public std::runtime_error {
public:
    LSQ_EndOfStream() : std::runtime_error("End of stream reached") {}
};

// -----------------------------
//  Cardinal-like helper (very unique names to avoid conflicts)
// -----------------------------
class LSQ_Cardinal {
public:
    enum LSQ_CardinalType { LSQ_FINITE = 0, LSQ_INFINITE = 1 };

private:
    LSQ_CardinalType lsq_type_;
    std::size_t lsq_finite_value_;

public:
    LSQ_Cardinal() : lsq_type_(LSQ_FINITE), lsq_finite_value_(0) {}
    LSQ_Cardinal(std::size_t v) : lsq_type_(LSQ_FINITE), lsq_finite_value_(v) {}
    LSQ_Cardinal(LSQ_CardinalType t) : lsq_type_(t), lsq_finite_value_(0) {}

    bool IsFinite() const { return lsq_type_ == LSQ_FINITE; }
    bool IsInfinite() const { return lsq_type_ == LSQ_INFINITE; }

    std::size_t GetFiniteValue() const {
        if (!IsFinite()) throw std::logic_error("Infinite cardinal has no finite value");
        return lsq_finite_value_;
    }
};

// -----------------------------
//  Base Sequence<T>
// -----------------------------
template <class T>
class LSQ_Sequence {
public:
    virtual ~LSQ_Sequence() {}
    virtual T GetFirst() const = 0;
    virtual T GetLast() const = 0;
    virtual T Get(int index) const = 0;
    virtual LSQ_Sequence<T>* GetSubsequence(int start, int end) const = 0;
    virtual std::size_t GetLength() const = 0;
    virtual LSQ_Sequence<T>* Append(T value) = 0;
    virtual LSQ_Sequence<T>* Prepend(T value) = 0;
    virtual LSQ_Sequence<T>* InsertAt(T value, int index) = 0;
    virtual LSQ_Sequence<T>* Concat(LSQ_Sequence<T>* other) const = 0;
};

// -----------------------------
//  LazySequence<T>
// -----------------------------
template <class T>
class LazySequence : public LSQ_Sequence<T> {
private:
    mutable std::map<int, T> lsq_materialized_;
    std::function<T(int)> lsq_generator_;
    T lsq_default_;
    LSQ_Cardinal lsq_length_;
    bool lsq_infinite_;

    void ValidateIndex(int index) const {
        if (index < 0) throw LSQ_IndexOutOfRange("Negative index: " + std::to_string(index));
        if (!lsq_infinite_ && (std::size_t)index >= lsq_length_.GetFiniteValue())
            throw LSQ_IndexOutOfRange("Index out of range");
    }

public:
    // 1) infinite default sequence
    LazySequence()
        : lsq_materialized_(),
        lsq_generator_([this](int) { return lsq_default_; }),
        lsq_default_(T()),
        lsq_length_(LSQ_Cardinal::LSQ_INFINITE),
        lsq_infinite_(true) {}

    // 2) from array (finite)
    LazySequence(const T* items, int count)
        : lsq_materialized_(), lsq_generator_(), lsq_default_(T()), lsq_length_((std::size_t)count), lsq_infinite_(false)
    {
        if (count < 0) throw std::invalid_argument("count must be >= 0");
        lsq_generator_ = [items, count](int idx) -> T {
            if (idx < 0 || idx >= count) throw LSQ_IndexOutOfRange();
            return items[idx];
            };
        for (int i = 0; i < count; ++i) lsq_materialized_[i] = items[i];
    }

    // 3) generator + default
    LazySequence(std::function<T(int)> gen, T defaultVal = T())
        : lsq_materialized_(), lsq_generator_(gen), lsq_default_(defaultVal), lsq_length_(LSQ_Cardinal::LSQ_INFINITE), lsq_infinite_(true)
    {
        if (!lsq_generator_) lsq_generator_ = [this](int) { return lsq_default_; };
    }

    // 4) from another sequence (finite)
    LazySequence(LSQ_Sequence<T>* seq)
        : lsq_materialized_(), lsq_generator_([seq](int idx) { return seq->Get(idx); }), lsq_default_(T()), lsq_length_(seq->GetLength()), lsq_infinite_(false)
    {}

    // -----------------------------
    //  API
    // -----------------------------
    T GetFirst() const override { return Get(0); }

    T GetLast() const override {
        if (lsq_infinite_) throw std::logic_error("Infinite sequence has no last element");
        return Get((int)lsq_length_.GetFiniteValue() - 1);
    }

    T Get(int index) const override {
        ValidateIndex(index);
        auto it = lsq_materialized_.find(index);
        if (it != lsq_materialized_.end()) return it->second;
        T v = lsq_generator_(index);
        lsq_materialized_[index] = v;
        return v;
    }

    //// ======= NEW: Set =======
    //// materialize/overwrite value at index
    void Set(int index, T value) {
        if (index < 0) throw LSQ_IndexOutOfRange("Negative index in Set");
        if (!lsq_infinite_ && (std::size_t)index >= lsq_length_.GetFiniteValue())
            throw LSQ_IndexOutOfRange("Index out of range in Set");
        lsq_materialized_[index] = value;
    }

    std::size_t GetLength() const override {
        if (lsq_infinite_) throw std::logic_error("Infinite sequence has no finite length");
        return lsq_length_.GetFiniteValue();
    }

    LSQ_Sequence<T>* GetSubsequence(int start, int end) const override {
        ValidateIndex(start);
        if (!lsq_infinite_) ValidateIndex(end);
        if (start > end) throw LSQ_IndexOutOfRange("start > end");
        int len = end - start + 1;
        std::vector<T> buf(len);
        for (int i = 0; i < len; ++i) buf[i] = Get(start + i);
        return new LazySequence<T>(buf.data(), len);
    }

    LSQ_Sequence<T>* Append(T value) override {
        if (lsq_infinite_) throw std::logic_error("Cannot append to infinite sequence");
        std::size_t old = lsq_length_.GetFiniteValue();
        auto gen = [this, value, old](int idx) -> T {
            if (idx == (int)old) return value;
            return this->Get(idx);
            };
        LazySequence<T>* ns = new LazySequence<T>(gen, lsq_default_);
        ns->lsq_infinite_ = false;
        ns->lsq_length_ = LSQ_Cardinal(old + 1);
        for (auto& p : lsq_materialized_) ns->lsq_materialized_[p.first] = p.second;
        ns->lsq_materialized_[(int)old] = value;
        return ns;
    }

    LSQ_Sequence<T>* Prepend(T value) override {
        auto gen = [this, value](int idx) -> T {
            if (idx == 0) return value;
            return this->Get(idx - 1);
            };
        LazySequence<T>* ns = new LazySequence<T>(gen, lsq_default_);
        ns->lsq_infinite_ = lsq_infinite_;
        if (!lsq_infinite_) ns->lsq_length_ = LSQ_Cardinal(lsq_length_.GetFiniteValue() + 1);
        for (auto& p : lsq_materialized_) ns->lsq_materialized_[p.first + 1] = p.second;
        ns->lsq_materialized_[0] = value;
        return ns;
    }

    LSQ_Sequence<T>* InsertAt(T value, int index) override {
        ValidateIndex(index);
        auto gen = [this, value, index](int i) -> T {
            if (i == index) return value;
            if (i < index) return this->Get(i);
            return this->Get(i - 1);
            };
        LazySequence<T>* ns = new LazySequence<T>(gen, lsq_default_);
        ns->lsq_infinite_ = lsq_infinite_;
        if (!lsq_infinite_) ns->lsq_length_ = LSQ_Cardinal(lsq_length_.GetFiniteValue() + 1);
        for (auto& p : lsq_materialized_) {
            if (p.first < index) ns->lsq_materialized_[p.first] = p.second;
            else ns->lsq_materialized_[p.first + 1] = p.second;
        }
        ns->lsq_materialized_[index] = value;
        return ns;
    }

    LSQ_Sequence<T>* Concat(LSQ_Sequence<T>* other) const override {
        LazySequence<T>* o = dynamic_cast<LazySequence<T>*>(other);
        if (!o) throw std::invalid_argument("Concat requires LazySequence");
        bool inf = lsq_infinite_ || o->lsq_infinite_;
        std::size_t myLen = lsq_infinite_ ? 0 : lsq_length_.GetFiniteValue();
        std::size_t oLen = o->lsq_infinite_ ? 0 : o->lsq_length_.GetFiniteValue();
        auto gen = [this, o, myLen](int idx) -> T {
            if (!this->lsq_infinite_ && (std::size_t)idx < myLen) return this->Get(idx);
            return o->Get(idx - (int)myLen);
            };
        LazySequence<T>* ns = new LazySequence<T>(gen, lsq_default_);
        ns->lsq_infinite_ = inf;
        if (!inf) ns->lsq_length_ = LSQ_Cardinal(myLen + oLen);
        return ns;
    }

    // Utilities used by other code (exposed)
    std::vector<T> GetRange(int start, int end) const {
        if (start > end) return std::vector<T>();
        std::vector<T> v;
        for (int i = start; i <= end; ++i) v.push_back(Get(i));
        return v;
    }

    std::size_t GetMaterializedCount() const { return lsq_materialized_.size(); }
    void ClearMaterialized() { lsq_materialized_.clear(); }
    bool IsInfinite() const { return lsq_infinite_; }
    LSQ_Cardinal GetCardinalLength() const { return lsq_length_; }
};

#endif // LAZYSEQUENCE_H
