#pragma once
#include "../include/ISet.h"

namespace {

class Set:public ISet {
public:
	Set();
    
    static RC setLogger(ILogger* const logger);

    virtual size_t getDim() const override;
	virtual size_t getSize() const override;
    virtual RC get(size_t index, IVector const*& val) const override;
	virtual RC findFirst(IVector const * const& pat, IVector::NORM n, double tol, IVector const *& val) const override;

	virtual RC insert(IVector const *& val, IVector::NORM n, double tol) override;

	virtual RC remove(size_t index) override;
	virtual RC remove(IVector const * const& pat, IVector::NORM n, double tol) override;

	virtual ~Set();

private:	
	Set(const ISet& other);
	Set& operator=(const ISet& other);

    static ILogger* _logger;

    double* _data;
    size_t _dim;
    size_t _allocated;
    size_t _size;

    size_t vecDataSize() const;

    RC findFirst(IVector const * const& pat, IVector::NORM n, double tol, IVector *& val, size_t& index) const;

    bool allocate();
protected:
};

}