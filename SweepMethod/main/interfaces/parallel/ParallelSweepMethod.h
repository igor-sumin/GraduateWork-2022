#pragma once

#include <utility>

#include "main/interfaces/parallel/ParallelInstrumental.h"
#include "main/interfaces/AbstractSweepMethod.h"
#include "main/interfaces/serial/SerialSweepMethod.h"


class ParallelSweepMethod : public ParallelInstrumental, public AbstractSweepMethod {
protected:
	matr A;
	vec b, y;

    // preprocessing Upper Left Corner of the matrix R
    void preULR(matr& R);

    // preprocessing Lower Right Corner of the matrix R
    void preLRR(matr& R);

public:
    ParallelSweepMethod() = default;

    ParallelSweepMethod(size_t n, size_t threadNum) : ParallelInstrumental(n, threadNum) {
        this->A = createThirdDiagMatrI();
        this->b = createVecN();
        this->y.assign(N, 0.);
    }

	ParallelSweepMethod(matr A_, vec b_) : A(std::move(A_)), b(std::move(b_)) {
        this->prepareData();
    }

    // Getting protected fields
    std::tuple<size_t, size_t, size_t, size_t, matr, vec, vec> getAllFields() const;

    // Setting protected fields
    void setAllFields(size_t N, size_t threadNum, size_t blockSize, size_t classicSize, const matr& A_, const vec& b_, const vec& y_);

    /*
     * 1.
     * Reduction of the matrix to columnar views
    */
    void transformation();

    /*
     *  2.
     *  Collect the equations in the matrix- R and the right part - partB
     *  for the vector of unknown - partY, interfere with parallel execution
    */
    std::pair<matr, vec> collectInterferElem();

    /*
     * 3.
     * Solve of the system of linear algebraic equations
     * R * partY = partB by the classical sweep method
    */
    vec collectPartY(const matr& R, const vec& partB);

    /*
     * 4.
     * Find the elements of the vector of unknowns - y,
     * that do not interfere with parallel execution
    */
    void collectNotInterferElem();

    /*
     * 5.
     * Combine interfering elements - partY with part of non-interfering - y
    */
    vec collectFullY(const vec& partY);

    /*
     * Parallel implementation of sweep method
     *
     * N         - dimension
     * threadNum - number of compute nodes
     * blockSize - dimension for one computing node
     * interSize - dimension of interfering elements
     * A         - tridiagonal matrix (NxN)
     * b         - the right part of the system of linear algebraic equations
     * y         - vector of unknowns
    */
    vec run() override;
};