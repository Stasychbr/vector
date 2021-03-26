#pragma once
#include "../include/IVector.h"

class Vector : public IVector {
private:
    size_t _dim;
    static ILogger* _logger;
    inline double* getDataArray();
public:
    static Vector* createVector(size_t dim, double const* const& pData);
    virtual IVector* clone() const override;
    virtual double const* getData() const override;

    static RC setLogger(ILogger* const logger);

    virtual RC getCord(size_t index, double& val) const override;
    virtual RC setCord(size_t index, double val) override;
    virtual RC scale(double multiplier) override;
    virtual size_t getDim() const override;

    virtual RC inc(IVector const* const& op) override;
    virtual RC dec(IVector const* const& op) override;

    static Vector* add(IVector const* const& op1, IVector const* const& op2);
    static Vector* sub(IVector const* const& op1, IVector const* const& op2);

    static double dot(IVector const* const& op1, IVector const* const& op2);
    static bool equals(IVector const* const& op1, IVector const* const& op2, NORM n, double tol);
    virtual double norm(NORM n) const override;

    virtual RC applyFunction(const std::function<double(double)>& fun) override;
    virtual RC foreach(const std::function<void(double)>& fun) const override;

    virtual size_t sizeAllocated() const override;

    virtual ~Vector();

protected:
    Vector(size_t dim);
};
