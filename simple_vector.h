#pragma once
#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <memory>

struct ReserveProxyObj {
    size_t capacity;
    explicit ReserveProxyObj(size_t cap) : capacity(cap) {}
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    SimpleVector(ReserveProxyObj reserve) {
        Reserve(reserve.capacity);
    }

    SimpleVector(const SimpleVector& other) : capacity_(other.capacity_), size_(other.size_) {
        capacity_ = other.capacity_;
        size_ = other.size_;
        items_ = new Type[size_];
        std::copy(other.begin(), other.end(), items_);
    }
    SimpleVector(SimpleVector&& other) : items_(other.items_), size_(other.size_), capacity_(other.capacity_){
        other.items_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) {
        items_ = (size == 0) ? nullptr : new Type[size];
        std::fill(items_, items_ + size, Type());
        size_ = size;
        capacity_ = size;
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) {
        items_ = new Type[size];
        std::fill(items_, items_ + size, value);
        size_ = size;
        capacity_ = size;
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) {
        items_ = new Type[init.size()];
        std::copy(init.begin(), init.end(), items_);
        size_ = init.size();
        capacity_ = size_;
    }

    ~SimpleVector() {
        delete[] items_;
    }

    SimpleVector& operator=(const SimpleVector& other) {
    if (this != &other) {
        if(other.IsEmpty()){
            size_ = 0;
            delete[] items_;
            items_ = nullptr;
            return *this;
        }
        SimpleVector temp(other); 
        swap(temp); 
    }
    return *this;
}

    SimpleVector& operator=(SimpleVector&& other){
        if(this != &other){
            delete[] items_;

            items_ = std::exchange(other.items_, nullptr);
            size_ = std::exchange(other.size_, 0);
            capacity_ = std::exchange(other.capacity_, 0);
        }
        return *this;
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return (size_ == 0) ? true : false;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("index out of range");
        }
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("index out of range");
        }
        return items_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size <= capacity_) {
            if (new_size < size_) {
                std::destroy(items_ + new_size, items_ + size_);
            }
            else if (new_size > size_) {
                for (size_t i = size_; i < new_size; ++i) {
                    items_[i] = Type();
                }
            }
            size_ = new_size;
            return;
        }
        size_t new_capacity = std::max(capacity_ * 2, new_size);

        Type* new_items = new Type[new_capacity]();
        std::move(items_, items_ + size_, new_items);
        
        delete[] items_;
        items_ = new_items;
        size_ = new_size;
        capacity_ = new_capacity;
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        if (size_ == 0) {
            return nullptr;
        }
        return &items_[0];
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        if (size_ == 0) {
            return nullptr;
        }
        return &items_[size_];
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        if (size_ == 0) {
            return nullptr;
        }
        return &items_[0];
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        if (size_ == 0) {
            return nullptr;
        }
        return &items_[size_];
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        if (size_ == 0) {
            return nullptr;
        }
        return &items_[0];
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        if (size_ == 0) {
            return nullptr;
        }
        return &items_[size_];
    }

    void PushBack(const Type& elem) {
        if (size_ < capacity_) {
            items_[size_] = elem;
            ++size_;
            return;
        }
        size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
        //тут как и во втором методе я не использую резерв, т.к он это делает для текущего оъекта, а тут целенаправленно копия создается
        Type* copy_items = new Type[new_capacity];
        std::copy(items_, items_ + size_, copy_items);
        copy_items[size_] = elem;
        delete[] items_;
        items_ = copy_items;
        capacity_ *= 2;
        ++size_;
    }

    void PushBack(Type&& elem){
        if(size_ < capacity_){
            items_[size_] = std::move(elem);
            ++size_;
            return;
        }
        size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
        Type* copy_items = new Type[new_capacity];
        std::move(items_, items_ + size_, copy_items);
        copy_items[size_] = std::move(elem);
        delete[] items_;
        items_ = copy_items;
        capacity_ = new_capacity;
        ++size_;
    }

    void PopBack() noexcept {
        assert (size_ > 0);
        --size_;
        items_[size_].~Type();
    }

    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(begin() <= pos || pos <= end());
        size_t pos_index = pos - begin();
        if (size_ == capacity_) {
            size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
            Type* new_items = new Type[new_capacity];
            std::copy(items_, items_ + pos_index, new_items);
            new_items[pos_index] = value;
            std::copy(items_ + pos_index, items_ + size_, new_items + pos_index + 1);
            delete[] items_;
            items_ = new_items;
            capacity_ = new_capacity;
        }
        else {
            std::copy_backward(items_ + pos_index, items_ + size_, items_ + size_ + 1);
            items_[pos_index] = value;
        }
        ++size_;
        return items_ + pos_index;
    }

    Iterator Insert(ConstIterator pos, Type&& value){
        assert(begin() <= pos || pos <= end());
        size_t pos_index = pos - begin();
        if(size_ == capacity_){
            size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
            Type* new_items = new Type[new_capacity];
            std::move(items_, items_ + size_, new_items);
            new_items[pos_index] = std::move(value);
            std::move(items_ + pos_index, items_ + size_, new_items + pos_index + 1);
            delete[] items_;
            items_ = new_items;
            capacity_ = new_capacity;
        }
        else{
            std::move_backward(items_ + pos_index, items_ + size_, items_ + size_ + 1);
            items_[pos_index] = std::move(value);
        }
        ++size_;
        return items_ + pos_index;
    }

    Iterator Erase(ConstIterator pos) {
        assert(begin() <= pos || pos <= end());
        size_t pos_erase = pos - begin();
        std::move(items_ + pos_erase + 1, items_ + size_, items_ + pos_erase);
        --size_;
        return begin() + pos_erase;
    }

    void swap(SimpleVector& other) noexcept {
        std::swap(items_, other.items_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            Type* copy_items = new Type[new_capacity];
            for (size_t i = 0; i < size_; ++i) {
                copy_items[i] = items_[i];
            }
            delete[] items_;
            items_ = copy_items;
            capacity_ = new_capacity;
        }
    }

private:
    Type* items_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}