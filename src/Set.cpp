#include <cstring>
#include <cmath>
#include "Set.h"

constexpr size_t basicSize = 100;
constexpr size_t maxEnlarger = 1000;

ILogger* Set::_logger = nullptr;

RC Set::setLogger(ILogger* const logger) {
    _logger = logger;
    return RC::SUCCESS;    
}

inline size_t Set::vecDataSize() const {
    return _dim * sizeof(double);
}

Set::Set() {
    _size = 0;
    _dim = 0;
    _allocated = 0;
    _data = nullptr;
}

size_t Set::getDim() const {
    return _dim;
}

size_t Set::getSize() const {
    return _size;
}

RC Set::get(size_t index, IVector const*& val) const {
#ifndef FAST_MATH
    if (index >= _size) {
        return RC::INDEX_OUT_OF_BOUND;
    }
#endif
    IVector* vector = IVector::createVector(_dim, _data + index * _dim);
#ifndef FAST_MATH
    if (!vector) {
        return RC::ALLOCATION_ERROR;
    }
#endif
    val = vector;
    return RC::SUCCESS;
}

RC Set::findFirst(IVector const * const& pat, IVector::NORM n, double tol, IVector *& val, size_t& index) const {
#ifndef FAST_MATH
    if (val->getDim() != _dim || pat->getDim() != _dim) {
        return RC::MISMATCHING_DIMENSIONS;
    }
#endif
    if (_size == 0) {
        return RC::VECTOR_NOT_FOUND;
    }
    for (int i = 0; i < _size; i++) {
        // will be optimized after IVector interface change
        for (int j = 0; j < _dim; j++) {
            val->setCord(j, *(_data + i * _dim + j));
        }
        val->dec(pat);
        if (val->norm(n) < tol) {
            index = i;
            return RC::SUCCESS;
        }
    }
    return RC::VECTOR_NOT_FOUND;
}

RC Set::findFirst(IVector const * const& pat, IVector::NORM n, double tol, IVector const *& val) const {
    size_t index = 0;
    if (_size == 0) {
        return RC::VECTOR_NOT_FOUND;
    }
    IVector* vec = IVector::createVector(_dim, _data);
    RC code = findFirst(pat, n, tol, vec, index);
    if (code != RC::SUCCESS) {
        delete vec;
        return code;
    }
    val = vec;
    return RC::SUCCESS;
}

bool Set::allocate() {
    double* newData = new double[fmin(2 * _allocated, maxEnlarger) * vecDataSize()];
    if (!newData) {
        return false;
    }
    if (_data) {
        memcpy(newData, _data, _allocated * vecDataSize());
        delete _data;
    }
    _allocated = fmin(2 * _allocated, maxEnlarger);
    _data = newData;
    return true;
}

RC Set::insert(IVector const *& val, IVector::NORM n, double tol) {
#ifndef FAST_MATH
    if (val->getDim() != _dim) {
        return RC::MISMATCHING_DIMENSIONS;
    }
#endif
    if (_allocated == 0) {
        _allocated = basicSize / 2;
        _dim = val->getDim();
        allocate();
    }
    if (_size == 0) {
        memmove(_data, val->getData(), _dim * sizeof(double)); 
        _size++;       
        return RC::SUCCESS;
    }
    if (_size == _allocated) {
        allocate();
    }
    IVector* curVec = IVector::createVector(_dim, _data);
    size_t index = 0;
    if (findFirst(val, n, tol, curVec, index) != RC::SUCCESS) {
        memmove(_data + _size * _dim, val->getData(), _dim * sizeof(double));
        _size++;
    }
    delete curVec;
    return RC::SUCCESS;
}

RC Set::remove(size_t index) {
#ifndef FAST_MATH
    if (index >= _size) {
        return RC::INDEX_OUT_OF_BOUND;
    }
#endif
    memcpy(_data + index * _dim, _data + (index + 1) * _dim, _allocated * vecDataSize() - vecDataSize() * (index + 1));
    _size--;
    return RC::SUCCESS;
}

RC Set::remove(IVector const * const& pat, IVector::NORM n, double tol) {
    size_t index = 0;
#ifndef FAST_MATH
    if (_size == 0) {
        return RC::VECTOR_NOT_FOUND;
    }
#endif
    IVector* vec = IVector::createVector(_dim, _data);
#ifndef FAST_MATH
    if (!vec) {
        return RC::ALLOCATION_ERROR;
    }
#endif
    RC code = findFirst(pat, n, tol, vec, index);
    delete vec;
    if (code == RC::SUCCESS) {
        remove(index);
    }
    return code;
}

Set::~Set() {
    delete _data;
}

RC ISet::setLogger(ILogger* const logger) {
    return Set::setLogger(logger);
}

ISet* ISet::createSet(ILogger* pLogger) {
    setLogger(pLogger);
    return new Set();
}

ISet* ISet::makeIntersection(ISet const * const& op1, ISet const * const& op2, IVector::NORM n, double tol) {
#ifndef FAST_MATH
    if (op1->getDim() != op2->getDim()) {
        return nullptr;
    }
#endif
    if (op1->getSize() == 0 || op2->getSize() == 0) {
        return createSet(nullptr);
    }

    const IVector* vec1 = nullptr;
    const IVector* vec2 = nullptr;
    // deep copying will be fixed after interface update
    if (op1->get(0, vec1) != RC::SUCCESS || op2->get(0, vec2) != RC::SUCCESS) {
        return nullptr;
    }
    
}

ISet* ISet::makeUnion(ISet const * const& op1, ISet const * const& op2, IVector::NORM n, double tol) {

}

ISet* ISet::sub(ISet const * const& op1, ISet const * const& op2, IVector::NORM n, double tol) {

}

ISet* ISet::symSub(ISet const * const& op1, ISet const * const& op2, IVector::NORM n, double tol) {

}
bool equals(ISet const * const& op1, ISet const * const& op2, IVector::NORM n, double tol) {

}

bool subSet(ISet const * const& op1, ISet const * const& op2, IVector::NORM n, double tol) {

}

ISet::~ISet() = default;