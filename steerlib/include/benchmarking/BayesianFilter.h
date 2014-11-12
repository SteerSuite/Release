//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*!

@file       BayesianFilter.h
@brief      Includes implementations of various Bayesian filters.

@mainpage   Bayesian Filter Library

@author     Jur van den Berg

<b>Bayesian Filter Library</b> is an easy-to-use C++ implementation of various
Bayesian filters for state estimation. Copy <tt>matrix.h</tt> and <tt>bayesianFilter.h</tt> 
to the appropriate directory and include <tt>bayesianFilter.h</tt> in your C++ project to use 
the functionality described below.

<b>Bayesian Filter Library</b> currenlty implements five types of Bayesian Filters:

- Kalman Filter
- Extended Kalman Filter
- Unscented Kalman Filter
- Particle Filter
- Ensemble Kalman Filter

Please click Modules to learn about these filters and the functionality contained
in <b>Bayesian Filter Library</b>.
*/

/*!
\defgroup kf Kalman Filter

The <b>Bayesian Filter Library</b> provides standard implementations of the Kalman Filter. The Kalman 
filter is the optimal and exact estimator for linear dynamics and measurement functions of the form 
<i>x = Ax + Bu + m, m ~ N(0, M)</i> and <i>z = Hx + n, n ~ N(0, N)</i>.

Functionality to perform a full filtering step, or separate control update and measurement update steps is
available for the Kalman Filter. The full step is equivalent to sequentially performing a control update and a
measurement update step. The running time of a full step is <i>O(xDim^3 + zDim xDim^2 + zDim^2 xDim + zDim^3)</i>. 
*/

/*!
\defgroup ekf Extended Kalman Filter

The <b>Bayesian Filter Library</b> provides standard implementations of the Extended Kalman Filter. For 
non-linear functions of the form <i>x = f(x,u) + m, m ~ N(0, M)</i> and <i>z = h(x) + n, n ~ N(0, N)</i> 
or, more generally, of the form <i>x = f(x,u,m), m ~ N(0, I)</i> and <i>z = h(x,n), n ~ N(0,I)</i>, the 
Extended Kalman Filter is an approximate estimator. The Extended Kalman Filter linearizes the dynamics and 
measurement functions at the maximum likelihood point using numerical differentiation (central differences) 
to compute the Jacobians. The step size for numerical differentiation is a parameter of the Extended Kalman Filter.

Functionality to perform a full filtering step, or separate control update and measurement update steps is
available for the Extended Kalman Filter. The full step is equivalent to sequentially performing a control 
update and a measurement update step. The running time of a full step is <i>O(xDim^3 + zDim xDim^2 + zDim^2 
xDim + zDim^3)</i>. 
*/

/*!
\defgroup ukf Unscented Kalman Filter

The <b>Bayesian Filter Library</b> provides an implementation of the Unscented Kalman Filter, following 
[E. Wan, R. van der Merwe; The Unscented Kalman Filter for Nonlinear Estimation; Adaptive Systems for Signal 
Processing, Communications, and Control Symposium; 2000]. It uses <i>2*(xDim + mDim + nDim) + 1</i> sigma 
points to propagate the mean and variance up to third-order accuracy for Gaussian inputs and non-linear 
dynamics and measurement functions of the general form <i>x = f(x,u,m), m ~ N(0, I)</i> and <i>z = h(x,n), 
n ~ N(0,I)</i>. Parameters of the algorithm are alpha, kappa (scaling parameters for the ``spread'' of the 
sigma points), and beta (kurtosis of prior distribution), of which the default values are 1e-3, 0, and 2, 
respectively (for Gaussian distributions, beta = 2 is optimal). 

The running time of the algorithm is <i>O(xDim^3 + zDim xDim^2 + zDim^2 xDim + zDim^3)</i>, which is the same
as for the (Extended) Kalman filter. Functionality to perform a full filtering step, or separate control update
and measurement update steps is available for the Unscented Kalman Filter. Sequentially performing a control 
update and a measurement update step is not entirely equivalent to peforming a full step, as the full step 
does not resample sigma points after the control update.
*/

/*!
\defgroup pf Particle Filter

The <b>Bayesian Filter Library</b> provides implementations of the Particle Filter. The standard algorithm
follows [S. Thrun, W. Burgard, D. Fox; Probabilistic Robotics; The MIT Press; 2005], and requires that the 
probability density p(z|x) is evaluable for given z and x, which is the case if the measurement function is 
linear in the measurement noise n, i.e. of the form h(x, n) = h(x) + N(x)*n, n ~ N(0,I), where N(x) is a matrix 
that may be an arbitrarily non-linear function of x. If the measurement function is not linear in n, the 
probability density p(z|x) is approximated by linearizing h in n = 0.

In case the posterior probability density p(x_t|x_{t-1}, z_t) can be sampled from and p(z_t, x_{t-1}) is 
evaluable, a Particle Filter with optimal proposal distribution, following [A. Doucet, S. Godsill, C. Andrieu; 
On sequential Monte Carlo sampling methods for Bayesian filtering; Statistics and Computing 10:197-208; 2000],
can be used. This filter requires that the dynamics function is linear in the motion noise m, i.e. of the form 
f(x, u, m) = f(x, u) + M(x, u)*m,  where M(x, u) is a matrix that may be an arbitrarily non-linear function of 
x and u, and the measurement function is linear, i.e. of the form <i>z = Hx + n, n ~ N(0, N)</i> for constant 
matrices H and N. If the dynamics function is not linear in m, it is approximated by linearizing f in m = 0.

The running time of the standard Particle filter is <i>O(zDim^3 + |X|(xDim + zDim^2))</i>, where <i>|X|</i> is 
the number of particles. The running time of the filter using the optimal proposal distribution is <i>O(|X|(xDim^3
+ zDim xDim^2 + zDim^2 xDim + zDim^3))</i>.
Functionality to perform a full filtering step, or separate control update and measurement update steps is 
available for the standard Particle Filter. The full step is equivalent to sequentially performing a control 
update and a measurement update step. The particle filter using the optimal proposal distribution cannot 
be broken down into a separate control and measurement update. For both filters, functionality is available for
low-variance sequential resampling to mitigate effects of sample impoverishment. 
*/

/*!
\defgroup enkf Ensemble Kalman Filter

The <b>Bayesian Filter Library</b> provides an implementation of the Ensemble Kalman Filter, roughly following 
[J. Mandel; A brief tutorial on the Ensemble Kalman Filter; Technical Report UC Denver; 2007], optimized for 
non-linear dynamics and measurement functions of the general form <i>x = f(x,u,m), m ~ N(0, I)</i> and <i>z 
= h(x,n), n ~ N(0,I)</i>. Ensemble Kalman filters are particularly suited in cases where the uncertainty can 
reasonably be assumed to be unimodal, and the state space is extremely high-dimensional, for instance in 
geophysical models of weather, ocean currents, or traffic.

The algorithm requires that the ensemble size (<i>|X|</i>) is bigger than the dimension of the measurement, i.e. 
<i>|X| > zDim</i>, to prevent singularities in the computation. The algorithm runs in <i>O(|X| xDim zDim + 
xDim zDim^2 + zDim^3 + |X| zDim^2)</i>. Note that <i>xDim</i> never appears squared or cubed, and that the 
running time can be reduced to <i>O(|X| xDim zDim)</i> if the elements of the measurement vector are independent 
of one another and can be incorporated one at a time. To this end, perform the control update step and the
measurement update step separately. A full Ensemble Kalman Filter step is equivalent to sequentially performing 
a control update and a measurement update step.
*/

/*!
\defgroup globalfunc Global Functionality

Functionality shared by the various filters.
*/

#ifndef __BAYESIANFILTER_H__
#define __BAYESIANFILTER_H__

#include <iterator>
#include <vector>
#include <stdlib.h>
#include "util/dmatrix.h"
#include "benchmarking/CompositeTechniqueEntropy.h"



/*!
*  @brief       Default step size for computing numerical derivatives
*
*  A numerical derivative of function f(x) at point x is computed as <i>(f(x+h)-f(x-h))/(2h)</i> 
*  where <i>h</i> is the step size.
*  \ingroup globalfunc
*/
static const double DEFAULTSTEPSIZE = 0.0009765625;

/*!
*  @brief       Default value for parameter alpha of the Unscented Kalman Filter.
*
*  Parameter alpha determines the spread of the sigma points in the UKF, and is usually
*  set to a small positive value.
*  \ingroup ukf
*/
static const double DEFAULTALPHA    = 1.0e-3;

/*!
*  @brief       Default value for parameter beta of the Unscented Kalman Filter.
*
*  Parameter beta is used to incorporate prior knowledge of the kurtosis of the distribution in 
*  the UKF. The default value beta = 2 is optimal for Gaussian distributions.
*  \ingroup ukf
*/
static const double DEFAULTBETA     = 2.0;

/*!
*  @brief       Default value for parameter kappa of the Unscented Kalman Filter.
*
*  Parameter kappa is a scaling parameter of the UKF, and is usually set to zero.
*  \ingroup ukf
*/
static const double DEFAULTKAPPA    = 0.0;

/*!
*  @brief       Randomly samples from the uniform distribution over range [0,1].
*  @returns     A uniform random number in [0,1].
*  \ingroup globalfunc
*/
inline double mrandom() {
	return ((double)rand())/((double)RAND_MAX);
	// return (double) (rand()*(RAND_MAX+1) + rand()) / (RAND_MAX*(RAND_MAX + 2));
}

/*!
*  @brief       Randomly samples from the univariate standard Gaussian distribution N(0,1).
*  @returns     A random number from the univariate standard Gaussian distribution N(0,1).
*  \ingroup globalfunc
*/
inline double normal() {
	double u, v, s(0);

	while (s == 0 || s > 1) {
		u = 2*mrandom()-1;
		v = 2*mrandom()-1;
		s = u*u + v*v;
	}

	return u * sqrt(-2*log(s)/s);
}

/*!
*  @brief       Randomly samples from the multivariate standard Gaussian distribution N(0,I).
*  @tparam      dim     The dimension of the distribution.
*  @returns     A random vector from the multivariate standard Gaussian distribution N(0,I).
*  \ingroup globalfunc
*/
inline Matrix sampleGaussian(size_t dim) {
	Matrix sample(dim);
	for (size_t j = 0; j < dim; ++j) {
		sample[j] = normal();
	}
	return sample;
}

/*!
*  @brief       Randomly samples from the multivariate Gaussian distribution with specified 
*               mean and variance.
*  @tparam      dim     The dimension of the distribution.
*  @param       mean    The mean of the distribution.
*  @param       var     The variance (covariance matrix) of the distribution.
*  @returns     A random vector from the specified Gaussian distribution.
*  \ingroup globalfunc
*/
inline Matrix sampleGaussian(const Matrix& mean, const Matrix& var) {
	size_t dim = mean.numRows();
	Matrix sample = sampleGaussian(dim);
	Matrix SVec(dim, dim), SVal(dim, dim);
	jacobi(var, SVec, SVal);
	for (size_t i = 0; i < dim; ++i) {
		if (SVal(i,i) < 0) {
			SVal(i,i) = 0;
		} else {
			SVal(i,i) = sqrt(SVal(i,i));
		}
	}
	return SVec * SVal * sample + mean;
}

/*!
*  @brief       Evaluates the probability density function of a multivariate Gaussian distribution 
with zero mean and specified variance at a specified point.
*  @tparam      dim     The dimension of the distribution.
*  @param       var     The variance (covariance matrix) of the distribution.
*  @param       x       The point at which the pdf is to be evaluated.
*  @returns     The probability density of the specified Gaussian distribution at the specified point.
*  \ingroup globalfunc
*/
inline double pdf(const Matrix& var, const Matrix& x) {
	return exp(-0.5*tr(~x*(var%x))) / sqrt(det(2*M_PI*var));
}

/*!
*  @brief       Evaluates the logarithm of the probability density function of a multivariate 
*               Gaussian distribution with zero mean and specified variance at a specified point.
*  @tparam      dim     The dimension of the distribution.
*  @param       var     The variance (covariance matrix) of the distribution.
*  @param       x       The point at which the log-pdf is to be evaluated.
*  @returns     The logarithm of the probability density of the specified Gaussian distribution at 
*               the specified point.
*  \ingroup globalfunc
*/
inline double logpdf(const Matrix& var, const Matrix& x) {
	size_t dim = x.numRows();
	return -0.5*tr(~x*(var%x)) - 0.5*dim*log(2*M_PI) - 0.5*log(det(var));
}

/*!
*  @brief       Computes the Jacobian of a specified function with three arguments with respect to the first
*               argument at a specified point using numerical differentiation.
*  @tparam      aDim    The dimension of the first argument of the specified function.
*  @tparam      bDim    The dimension of the second argument of the specified function.
*  @tparam      cDim    The dimension of the third argument of the specified function.
*  @tparam      yDim    The dimension of the return value of the specified function.
*  @param       a       The value of the first argument at which the Jacobian is to be computed.
*  @param       b       The value of the second argument at which the Jacobian is to be computed.
*  @param       c       The value of the third argument at which the Jacobian is to be computed.
*  @param       f       A pointer to the function of the form <i>y = f(a,b,c)</i> of which the Jacobian
*                       is to be computed.
*  @param       jStep   The step size used for numerical differentiation (optional).
*  @returns     The Jacobian matrix <i>df/da</i> evaluated at <i>(a,b,c)</i>.
*  \ingroup globalfunc
*/
inline Matrix jacobian1(const Matrix& a, const Matrix& b, const Matrix& c, size_t yDim,
						Matrix (*f)(const Matrix&, const Matrix&, const Matrix&), double jStep = DEFAULTSTEPSIZE) 
{
	size_t aDim = a.numRows();
	size_t bDim = b.numRows();
	size_t cDim = c.numRows();

	Matrix A(yDim,aDim);
	Matrix ar(a), al(a);
	for (size_t i = 0; i < aDim; ++i) {
		ar[i] += jStep; al[i] -= jStep;
		A.insert(0,i, (f(ar, b, c) - f(al, b, c)) / (2*jStep));
		ar[i] = al[i] = a[i];
	}
	return A;
}

/*!
*  @brief       Computes the Jacobian of a specified function with two arguments with respect to the first
*               argument at a specified point using numerical differentiation.
*  @tparam      aDim    The dimension of the first argument of the specified function.
*  @tparam      bDim    The dimension of the second argument of the specified function.
*  @tparam      yDim    The dimension of the return value of the specified function.
*  @param       a       The value of the first argument at which the Jacobian is to be computed.
*  @param       b       The value of the second argument at which the Jacobian is to be computed.
*  @param       f       A pointer to the function of the form <i>y = f(a,b)</i> of which the Jacobian
*                       is to be computed.
*  @param       jStep   The step size used for numerical differentiation (optional).
*  @returns     The Jacobian matrix <i>df/da</i> evaluated at <i>(a,b)</i>.
*  \ingroup globalfunc
*/
inline Matrix jacobian1(const Matrix& a, const Matrix& b, size_t yDim,
						Matrix (*f)(const Matrix&, const Matrix&), double jStep = DEFAULTSTEPSIZE) 
{
	size_t aDim = a.numRows();
	size_t bDim = b.numRows();

	Matrix A(yDim,aDim);
	Matrix ar(a), al(a);
	for (size_t i = 0; i < aDim; ++i) {
		ar[i] += jStep; al[i] -= jStep;
		A.insert(0,i, (f(ar, b) - f(al, b)) / (2*jStep));
		ar[i] = al[i] = a[i];
	}
	return A;
}

/*!
*  @brief       Computes the Jacobian of a specified function with one argument with respect to the 
*               argument at a specified point using numerical differentiation.
*  @tparam      aDim    The dimension of the argument of the specified function.
*  @tparam      yDim    The dimension of the return value of the specified function.
*  @param       a       The value of the argument at which the Jacobian is to be computed.
*  @param       f       A pointer to the function of the form <i>y = f(a)</i> of which the Jacobian
*                       is to be computed.
*  @param       jStep   The step size used for numerical differentiation (optional).
*  @returns     The Jacobian matrix <i>df/da</i> evaluated at <i>a</i>.
*  \ingroup globalfunc
*/
inline Matrix jacobian1(const Matrix& a, size_t yDim, Matrix (*f)(const Matrix&), double jStep = DEFAULTSTEPSIZE) 
{
	size_t aDim = a.numRows();
	Matrix A(yDim,aDim);
	Matrix ar(a), al(a);
	for (size_t i = 0; i < aDim; ++i) {
		ar[i] += jStep; al[i] -= jStep;
		A.insert(0,i, (f(ar) - f(al)) / (2*jStep));
		ar[i] = al[i] = a[i];
	}
	return A;
}

/*!
*  @brief       Computes the Jacobian of a specified function with three argument with respect to the second
*               parameter at a specified point using numerical differentiation.
*  @tparam      aDim    The dimension of the first argument of the specified function.
*  @tparam      bDim    The dimension of the second argument of the specified function.
*  @tparam      cDim    The dimension of the third argument of the specified function.
*  @tparam      yDim    The dimension of the return value of the specified function.
*  @param       a       The value of the first argument at which the Jacobian is to be computed.
*  @param       b       The value of the second argument at which the Jacobian is to be computed.
*  @param       c       The value of the third argument at which the Jacobian is to be computed.
*  @param       f       A pointer to the function of the form <i>y = f(a,b,c)</i> of which the Jacobian
*                       is to be computed.
*  @param       jStep   The step size used for numerical differentiation (optional).
*  @returns     The Jacobian matrix <i>df/db</i> evaluated at <i>(a,b,c)</i>.
*  \ingroup globalfunc
*/
inline Matrix jacobian2(const Matrix& a, const Matrix& b, const Matrix& c, size_t yDim,
						Matrix (*f)(const Matrix&, const Matrix&, const Matrix&), double jStep = DEFAULTSTEPSIZE) 
{
	size_t aDim = a.numRows();
	size_t bDim = b.numRows();
	size_t cDim = c.numRows();

	Matrix B(yDim,bDim);
	Matrix br(b), bl(b);
	for (size_t i = 0; i < bDim; ++i) {
		br[i] += jStep; bl[i] -= jStep;
		B.insert(0,i, (f(a, br, c) - f(a, bl, c)) / (2*jStep));
		br[i] = bl[i] = b[i];
	}
	return B;
}

/*!
*  @brief       Computes the Jacobian of a specified function with two arguments with respect to the second
*               argument at a specified point using numerical differentiation.
*  @tparam      aDim    The dimension of the first argument of the specified function.
*  @tparam      bDim    The dimension of the second argument of the specified function.
*  @tparam      yDim    The dimension of the return value of the specified function.
*  @param       a       The value of the first argument at which the Jacobian is to be computed.
*  @param       b       The value of the second argument at which the Jacobian is to be computed.
*  @param       f       A pointer to the function of the form <i>y = f(a,b)</i> of which the Jacobian
*                       is to be computed.
*  @param       jStep   The step size used for numerical differentiation (optional).
*  @returns     The Jacobian matrix <i>df/db</i> evaluated at <i>(a,b)</i>.
*  \ingroup globalfunc
*/
inline Matrix jacobian2(const Matrix& a, const Matrix& b, size_t yDim, Matrix (*f)(const Matrix&, const Matrix&), double jStep = DEFAULTSTEPSIZE) 
{
	size_t aDim = a.numRows();
	size_t bDim = b.numRows();

	Matrix B(yDim,bDim);
	Matrix br(b), bl(b);
	for (size_t i = 0; i < bDim; ++i) {
		br[i] += jStep; bl[i] -= jStep;
		B.insert(0,i, (f(a, br) - f(a, bl)) / (2*jStep));
		br[i] = bl[i] = b[i];
	}
	return B;
}

/*!
*  @brief       Computes the Jacobian of a specified function with three arguments with respect to the third
*               argument at a specified point using numerical differentiation.
*  @tparam      aDim    The dimension of the first argument of the specified function.
*  @tparam      bDim    The dimension of the second argument of the specified function.
*  @tparam      cDim    The dimension of the third argument of the specified function.
*  @tparam      yDim    The dimension of the return value of the specified function.
*  @param       a       The value of the first argument at which the Jacobian is to be computed.
*  @param       b       The value of the second argument at which the Jacobian is to be computed.
*  @param       c       The value of the third argument at which the Jacobian is to be computed.
*  @param       f       A pointer to the function of the form <i>y = f(a,b,c)</i> of which the Jacobian
*                       is to be computed.
*  @param       jStep   The step size used for numerical differentiation (optional).
*  @returns     The Jacobian matrix <i>df/dc</i> evaluated at <i>(a,b,c)</i>.
*  \ingroup globalfunc
*/
inline Matrix jacobian3(const Matrix& a, const Matrix& b, const Matrix& c, size_t yDim,
						Matrix (*f)(const Matrix&, const Matrix&, const Matrix&), double jStep = DEFAULTSTEPSIZE) 
{
	size_t cDim = c.numRows();

	Matrix C(yDim,cDim);
	Matrix cr(b), cl(b);
	for (size_t i = 0; i < cDim; ++i) {
		cr[i] += jStep; cl[i] -= jStep;
		C.insert(0,i, (f(a, b, cr) - f(a, b, cl)) / (2*jStep));
		cr[i] = cl[i] = c[i];
	}
	return C;
}


// Ensemble Kalman Filter and Smoother //

/*!
*  @brief       Performs the control update step of the Ensemble Kalman Filter.
*  @tparam      xDim    The dimension of the state.
*  @tparam      uDim    The dimension of the control input.
*  @tparam      mDim    The dimension of the motion noise.
*  @param[in,out]       X       In: the ensemble of states defining the prior distribution. 
*                              Out: the ensemble of states defining the posterior distribution.
*  @param       u       The control input that is applied.
*  @param       f       A pointer to the dynamics function of the form <i>x = f(x,u,m), m ~ N(0,I)</i>, that is 
*                       used to perform the control update step.
*  \ingroup enkf
*/
inline void enkfControlUpdate(std::vector<Matrix>& X, const Matrix& u, size_t mDim,
		SteerLib::CompositeTechniqueEntropy * entopy)
{
	size_t xDim = X[0].numRows();
	size_t uDim = u.numRows();

	// run ensemble members through f
	for (size_t i = 0; i < X.size(); ++i) {
		if (i == 684)
			i = 684;
		X[i] = entopy->m_fHat(X[i], u, sampleGaussian(mDim));
	}
}

/*!
*  @brief       Performs the control update step of the Ensemble Kalman Smoother.
*  @tparam      xDim    The dimension of the state.
*  @tparam      uDim    The dimension of the control input.
*  @tparam      mDim    The dimension of the motion noise.
*  @param[in,out]       Xs      In: the list of ensembles of states defining the prior distributions.
*                                   The ensemble with index <i>t</i> corresponds to time step <i>t</i>.
*                              Out: the list of ensembles of states defining the posterior distributions.
*                                   A new ensemble has been added to the back of the list.
*  @param       u       The control input that is applied.
*  @param       f       A pointer to the dynamics function of the form <i>x = f(x,u,m), m ~ N(0,I)</i>, that is 
*                       used to perform the control update step.
*  \ingroup enkf
*/

inline void enksControlUpdate(std::vector< std::vector<Matrix> >& Xs, const Matrix& u, size_t mDim, Matrix (*f)(const Matrix&, const Matrix&, const Matrix&)) 
{
	size_t xDim = Xs.back()[0].numRows();
	size_t uDim = u.numRows();
	
	// run ensemble members through f
	std::vector<Matrix> X(Xs.back().size()); 
	for (size_t i = 0; i < X.size(); ++i) {
		X[i] = f(Xs.back()[i], u, sampleGaussian(mDim));
	}

	Xs.push_back(X);
}

/*!
*  @brief       Performs the measurement update step of the Ensemble Kalman Filter.
*  @tparam      xDim    The dimension of the state.
*  @tparam      zDim    The dimension of the measurement.
*  @tparam      nDim    The dimension of the measurement noise.
*  @param[in,out]       X       In: the ensemble of states defining the prior distribution. 
*                              Out: the ensemble of states defining the posterior distribution.
*                              It is required that the ensemble size (<i>|X|</i>) is bigger than the dimension 
*                              of the measurement, i.e. <i>|X| > zDim</i>, to prevent singularities in the 
*                              computation. 
*  @param       z       The measurement that is incorporated.
*  @param       h       A pointer to the measurement function of the form <i>z = h(x,n), n ~ N(0,I)</i>, that is 
*                       used to perform the measurement update step.
*  \ingroup enkf
*/
inline void enkfMeasurementUpdate(std::vector<Matrix>& X, const Matrix& z,
		size_t nDim, SteerLib::CompositeTechniqueEntropy * entropy2)
{
	size_t xDim = X[0].numRows();
	size_t zDim = z.numRows();
	
	// compute mean ensemble -- O(N*xDim)
	Matrix xHat = zeros(xDim);
	for (size_t i = 0; i < X.size(); ++i) {
		xHat += X[i];
	}
	xHat /= (double)X.size();

	// run ensemble members through h -- O(N*zDim)
	std::vector<Matrix> Z(X.size());
	for (size_t i = 0; i < Z.size(); ++i) {
		Z[i] = entropy2->h(X[i], sampleGaussian(nDim));
	}

	// compute mean measurement -- O(N*zDim)
	Matrix zHat = zeros(zDim);
	for (size_t i = 0; i < Z.size(); ++i) {
		zHat += Z[i];
		assert(zHat[2] == zHat[2]);
		// If this fails then we have some NaNs in the data
	}
	zHat /= (double)Z.size();

	// calculate variance -- O(N*zDim*zDim)
	Matrix Pzz = zeros(zDim,zDim);
	for (size_t i = 0; i < Z.size(); ++i) {
		Pzz += (Z[i] - zHat)*~(Z[i] - zHat);
	}

	// calculate cross-covariance -- O(N*xDim*zDim)
	Matrix Pxz = zeros(xDim,zDim);
	for (size_t i = 0; i < Z.size(); ++i) {
		Pxz += (X[i] - xHat)*~(Z[i] - zHat);
	}

	// compute Kalman gain -- O(xDim*zDim*zDim + zDim^3)
	Matrix K = Pxz / Pzz;

	// update ensemble members -- O(N*xDim*zDim)
	for (size_t i = 0; i < X.size(); ++i) {
		X[i] += K*(z - Z[i]);
	}
}

/*!
*  @brief       Performs the measurement update step of the Ensemble Kalman Smoother.
*  @tparam      xDim    The dimension of the state.
*  @tparam      zDim    The dimension of the measurement.
*  @tparam      nDim    The dimension of the measurement noise.
*  @param[in,out]       Xs      In: the list of ensembles of states defining the prior distributions. 
*                                   The ensemble with index <i>t</i> corresponds to time step <i>t</i>.
*                              Out: the list of ensembles of states defining the posterior distributions.
*                              It is required that the ensemble sizes (<i>|Xs(t)|</i>) are bigger than the dimension 
*                              of the measurement, i.e. <i>|Xs(t)| > zDim</i>, to prevent singularities in the 
*                              computation. 
*  @param       z       The measurement that is incorporated.
*  @param       h       A pointer to the measurement function of the form <i>z = h(x,n), n ~ N(0,I)</i>, that is 
*                       used to perform the measurement update step.
*  \ingroup enkf
*/
inline void enksMeasurementUpdate(std::vector< std::vector<Matrix> >& Xs, const Matrix& z, size_t nDim, Matrix (*h)(const Matrix&, const Matrix&)) 
{
	size_t xDim = Xs.back()[0].numRows();
	size_t zDim = z.numRows();

	// run ensemble members through h -- O(N*zDim)
	std::vector<Matrix> Z(Xs.back().size());
	for (size_t i = 0; i < Z.size(); ++i) {
		Z[i] = h(Xs.back()[i], sampleGaussian(nDim));
	}

	// compute mean measurement -- O(N*zDim)
	Matrix zHat = zeros(zDim);
	for (size_t i = 0; i < Z.size(); ++i) {
		zHat += Z[i];
	}
	zHat /= (double)Z.size();

	// calculate variance -- O(N*zDim*zDim)
	Matrix Pzz = zeros(zDim,zDim);
	for (size_t i = 0; i < Z.size(); ++i) {
		Pzz += (Z[i] - zHat)*~(Z[i] - zHat);
	}

	for (size_t t = 0; t < Xs.size(); ++t) {

		// compute mean ensemble -- O(N*xDim)
		Matrix xHat = zeros(xDim);
		for (size_t i = 0; i < Xs[t].size(); ++i) {
			xHat += Xs[t][i];
		}
		xHat /= (double)Xs[t].size();

		// calculate cross-covariance -- O(N*xDim*zDim)
		Matrix Pxz = zeros(xDim,zDim);
		for (size_t i = 0; i < Z.size(); ++i) {
			Pxz += (Xs[t][i] - xHat)*~(Z[i] - zHat);
		}

		// compute Kalman gain -- O(xDim*zDim*zDim + zDim^3)
		Matrix K = Pxz / Pzz;

		// update ensemble members -- O(N*xDim*zDim)
		for (size_t i = 0; i < Xs[t].size(); ++i) {
			Xs[t][i] += K*(z - Z[i]);
		}
	}
}

/*!
*  @brief       Performs a full step of the Ensemble Kalman Filter. 
*  @tparam      xDim    The dimension of the state.
*  @tparam      uDim    The dimension of the control input.
*  @tparam      mDim    The dimension of the motion noise.
*  @tparam      zDim    The dimension of the measurement.
*  @tparam      nDim    The dimension of the measurement noise.
*  @param[in,out]       X       In: the ensemble of states defining the prior distribution. 
*                              Out: the ensemble of states defining the posterior distribution.
*                              It is required that the ensemble size (<i>|X|</i>) is bigger than the dimension 
*                              of the measurement, i.e. <i>|X| > zDim</i>, to prevent singularities in the 
*                              computation. 
*  @param       u       The control input that is applied.
*  @param       z       The measurement that is incorporated.
*  @param       f       A pointer to the dynamics function of the form <i>x = f(x,u,m), m ~ N(0,I)</i>, that is 
*                       used to perform the control update step.
*  @param       h       A pointer to the measurement function of the form <i>z = h(x,n), n ~ N(0,I)</i>, that is 
*                       used to perform the measurement update step.
*  @note        This function is equivalent to sequentially calling enkfControlUpdate and enkfMeasurementUpdate.
*  \ingroup enkf
*/

inline void ensembleKalmanFilter(std::vector<Matrix>& X, const Matrix& u, const Matrix& z, size_t mDim,
		SteerLib::CompositeTechniqueEntropy * entopy,
								 size_t nDim, SteerLib::CompositeTechniqueEntropy * entropy2) {
	enkfControlUpdate(X, u, mDim, entopy);
	enkfMeasurementUpdate(X, z, nDim, entropy2);
}

/*!
*  @brief       Performs a full step of the Ensemble Kalman Smoother. 
*  @tparam      xDim    The dimension of the state.
*  @tparam      uDim    The dimension of the control input.
*  @tparam      mDim    The dimension of the motion noise.
*  @tparam      zDim    The dimension of the measurement.
*  @tparam      nDim    The dimension of the measurement noise.
*  @param[in,out]       Xs      In: the list of ensembles of states defining the prior distributions.
*                                   The ensemble with index <i>i</i> corresponds to time step <i>t - i</i>,
*                                   where <i>t</i> is the current time.
*                              Out: the list of ensembles of states defining the posterior distributions.
*                                   A new ensemble has been added to the front of the list, so the index of each
*                                   ensemble has shifted 1, which corresponds to time progressing one step.
*                              It is required that the ensemble sizes (<i>|Xs(t)|</i>) is bigger than the dimension 
*                              of the measurement, i.e. <i>|Xs(t)| > zDim</i>, to prevent singularities in the 
*                              computation. 
*  @param       u       The control input that is applied.
*  @param       z       The measurement that is incorporated.
*  @param       f       A pointer to the dynamics function of the form <i>x = f(x,u,m), m ~ N(0,I)</i>, that is 
*                       used to perform the control update step.
*  @param       h       A pointer to the measurement function of the form <i>z = h(x,n), n ~ N(0,I)</i>, that is 
*                       used to perform the measurement update step.
*  @note        This function is equivalent to sequentially calling enksControlUpdate and enksMeasurementUpdate.
*  \ingroup enkf
*/

inline void ensembleKalmanSmoother(std::vector< std::vector<Matrix> >& Xs, const Matrix& u, const Matrix& z, 
								   size_t mDim, Matrix (*f)(const Matrix&, const Matrix&, const Matrix&), 
								   size_t nDim, Matrix (*h)(const Matrix&, const Matrix&)) {
	enksControlUpdate(Xs, u, mDim, f);
	enksMeasurementUpdate(Xs, z, nDim, h);
}

/*!
*  @brief       Performs a full step of the Particle Filter using the optimal proposal distribution. 
*  @tparam      xDim    The dimension of the state.
*  @tparam      uDim    The dimension of the control input.
*  @tparam      mDim    The dimension of the motion noise.
*  @tparam      zDim    The dimension of the measurement.
*  @param[in,out]       X       In: the set of particles defining the prior distribution of the state. 
*                              Out: the set of particles defining the posterior distribution of the state.
*  @param[in,out]       W       In: the weights of the particles of the prior distribution. 
*                              Out: the weights of the particles of the posterior distribution.
*                              It is required that <i>|W| = |X|</i>, and that the weights are normalized, i.e. 
<i>sum(W) = 1</i>.
*  @param       u       The control input that is applied.
*  @param       z       The measurement that is incorporated.
*  @param       f       A pointer to the dynamics function of the form <i>x = f(x,u,m), m ~ N(0,I)</i>, that is 
*                       used to perform the control update step. For the proposal distribution to be optimal, 
*                       <i>f</i> must be linear in the motion noise m, i.e. of the form <i>f(x, u, m) = f(x, u) 
+ M(x, u)*m</i>,  where <i>M(x, u)</i> is a matrix that may be an arbitrarily non-linear 
function of x and u. If the dynamics function is not linear in m, it is approximated by 
linearizing <i>f</i> in <i>m = 0</i>.
*  @param       H       The measurement matrix of the linear measurement function of the form <i>z = Hx + n, 
*                       n ~ N(0, N)</i> that is used to perform the measurement update step.
*  @param       N       The measurement noise covariance matrix of the linear measurement function of the form 
*                       <i>z = Hx + n, n ~ N(0, N)</i> that is used to perform the measurement update step.
*  @param       jStep   Parameter determining the step size used for numerical differentiation (optional).
*  \ingroup pf
*/
inline void optimalParticleFilter(std::vector<Matrix>& X, std::vector<double>& W, const Matrix& u, const Matrix& z, size_t mDim, 
								  Matrix (*f)(const Matrix&, const Matrix&, const Matrix&), const Matrix& H, const Matrix& N, double jStep = DEFAULTSTEPSIZE) 
{
	size_t xDim = X[0].numRows();

	double maxLogW = log(0.0);
	for (size_t i = 0; i < X.size(); ++i) {
		Matrix M = jacobian3(X[i], u, zeros(mDim), xDim, f, jStep);
		Matrix Sigma = M*~M; // O(|X| xDim^3)

		X[i] = f(X[i], u, zeros(mDim));

		Matrix SigmaZ = H*Sigma*~H + N; // O(|X| xDim^2 zDim)
		Matrix zHat = H*X[i];

		W[i] = log(W[i]) + logpdf(SigmaZ, z - zHat); // O(|X| zDim^3)
		if (W[i] > maxLogW) {
			maxLogW = W[i];
		}

		Matrix K = (Sigma*~H)/SigmaZ;
		X[i] += K*(z - zHat);
		Sigma -= K*(H*Sigma);

		X[i] = sampleGaussian(X[i], Sigma); // O(|X| xDim^3)
	}

	// scale W[i] such that exp(maxLogW) == 1 for numerical stability
	double totalW = 0;
	for (size_t i = 0; i < X.size(); ++i) {
		W[i] = exp(W[i] - maxLogW);
		totalW += W[i];
	}

	// normalize weights
	for (size_t i = 0; i < X.size(); ++i) {
		W[i] /= totalW;
	}
}

/*!
*  @brief       Performs a full step of the Particle Filter using the optimal proposal distribution. 
*  @tparam      xDim    The dimension of the state.
*  @tparam      uDim    The dimension of the control input.
*  @tparam      mDim    The dimension of the motion noise.
*  @tparam      zDim    The dimension of the measurement.
*  @param[in,out]       X       In: the set of particles defining the prior distribution of the state. 
*                              Out: the set of particles defining the posterior distribution of the state.
*  @param[in,out]       W       In: the weights of the particles of the prior distribution. 
*                              Out: the weights of the particles of the posterior distribution.
*                              It is required that <i>|W| = |X|</i>, and that the weights are normalized, i.e. 
<i>sum(W) = 1</i>.
*  @param       u       The control input that is applied.
*  @param       z       The measurement that is incorporated.
*  @param       f       A pointer to the dynamics function of the form <i>x = f(x, u) + m, m ~ N(0, M)</i>, that 
*                       is used to perform the control update step. 
*  @param       M       The motion noise covariance matrix of the dynamics function of the form 
*                       <i>f(x, u) + m, m ~ N(0, M)</i> that is used to perform the control update step.
*  @param       H       The measurement matrix of the linear measurement function of the form <i>z = Hx + n, 
*                       n ~ N(0, N)</i> that is used to perform the measurement update step.
*  @param       N       The measurement noise covariance matrix of the linear measurement function of the form 
*                       <i>z = Hx + n, n ~ N(0, N)</i> that is used to perform the measurement update step.
*  \ingroup pf
*/
inline void optimalParticleFilter(std::vector<Matrix>& X, std::vector<double>& W, const Matrix& u, const Matrix& z, 
								  Matrix (*f)(const Matrix&, const Matrix&), const Matrix& M, const Matrix& H, const Matrix& N) 
{
	size_t zDim = z.numRows();

	Matrix SigmaZ = H*M*~H + N;
	double constant = -0.5*zDim*log(2*M_PI) - 0.5*log(det(SigmaZ));
	Matrix SigmaZinv = !SigmaZ;

	double maxLogW = log(0.0);
	for (size_t i = 0; i < X.size(); ++i) {
		X[i] = f(X[i], u);

		Matrix zHat = H*X[i];

		W[i] = log(W[i]) - 0.5*tr(~(z - zHat)*SigmaZinv*(z - zHat)) + constant;
		if (W[i] > maxLogW) {
			maxLogW = W[i];
		}

		Matrix K = (M*~H)*SigmaZinv;
		X[i] = sampleGaussian(X[i] + K*(z - zHat), M - K*H*M);
	}

	// scale W[i] such that exp(maxLogW) == 1 for numerical stability
	double totalW = 0;
	for (size_t i = 0; i < X.size(); ++i) {
		W[i] = exp(W[i] - maxLogW);
		totalW += W[i];
	}

	// normalize weights
	for (size_t i = 0; i < X.size(); ++i) {
		W[i] /= totalW;
	}
}

/*!
*  @brief       Performs a control update step of the standard Particle Filter. 
*  @tparam      xDim    The dimension of the state.
*  @tparam      uDim    The dimension of the control input.
*  @tparam      mDim    The dimension of the motion noise.
*  @param[in,out]       X       In: the set of particles defining the prior distribution of the state. 
*                              Out: the set of particles defining the posterior distribution of the state.
*  @param       u       The control input that is applied.
*  @param       f       A pointer to the dynamics function of the form <i>x = f(x, u, m), m ~ N(0, I)</i>, that 
*                       is used to perform the control update step. 
*  @note        The weights of the particles do not change in the control update step, so they need not be passed
*               to this function.
*  \ingroup pf
*/
inline void pfControlUpdate(std::vector<Matrix>& X, const Matrix& u, size_t mDim, Matrix (*f)(const Matrix&, const Matrix&, const Matrix&)) {
	// run particles through f
	for (size_t i = 0; i < X.size(); ++i) {
		X[i] = f(X[i], u, sampleGaussian(mDim)); // O(|X| xDim)
	}
}

/*!
*  @brief       Performs a measurement update step of the standard Particle Filter. 
*  @tparam      xDim    The dimension of the state.
*  @tparam      zDim    The dimension of the measurement.
*  @tparam      nDim    The dimension of the measurement noise.
*  @param[in,out]       X       In: the set of particles defining the prior distribution of the state. 
*                              Out: the set of particles defining the posterior distribution of the state.
*  @param[in,out]       W       In: the weights of the particles of the prior distribution. 
*                              Out: the weights of the particles of the posterior distribution.
*                              It is required that <i>|W| = |X|</i>, and that the weights are normalized, i.e. 
*                              <i>sum(W) = 1</i>.
*  @param       z       The measurement that is incorporated.
*  @param       h       A pointer to the measurement function of the form <i>z = h(x, n), n ~ N(0, I)</i>, that 
*                       is used to perform the control update step. For the particle filter to function correctly, 
*                       <i>h</i> must be linear in the measurement noise n, i.e. of the form <i>h(x, n) = h(x) 
*                       + N(x) n</i>,  where <i>N(x)</i> is a matrix that may be an arbitrarily non-linear 
*                       function of x. If the measurement function is not linear in <i>n</i>, it is approximated 
*                       by linearizing <i>h</i> in <i>n = 0</i>.
*  @param       jStep   Parameter determining the step size used for numerical differentiation (optional).
*  \ingroup pf
*/
inline void pfMeasurementUpdate(std::vector<Matrix>& X, std::vector<double>& W, const Matrix& z, size_t nDim, Matrix (*h)(const Matrix&, const Matrix&), double jStep = DEFAULTSTEPSIZE) 
{
	size_t zDim = z.numRows();

	double maxLogW = log(0.0);
	for (size_t i = 0; i < X.size(); ++i) {
		Matrix N = jacobian2(X[i], zeros(nDim), zDim, h, jStep);
		W[i] = log(W[i]) + logpdf(N*~N, z - h(X[i], zeros(nDim))); // O(|X| zDim^3)
		if (W[i] > maxLogW) {
			maxLogW = W[i];
		}
	}

	// scale W[i] such that exp(maxLogW) == 1 for numerical stability
	double totalW = 0;
	for (size_t i = 0; i < X.size(); ++i) {
		W[i] = exp(W[i] - maxLogW);
		totalW += W[i];
	}

	// normalize weights
	for (size_t i = 0; i < X.size(); ++i) {
		W[i] /= totalW;
	}
}

/*!
*  @brief       Performs a measurement update step of the standard Particle Filter. 
*  @tparam      xDim    The dimension of the state.
*  @tparam      zDim    The dimension of the measurement.
*  @param[in,out]       X       In: the set of particles defining the prior distribution of the state. 
*                              Out: the set of particles defining the posterior distribution of the state.
*  @param[in,out]       W       In: the weights of the particles of the prior distribution. 
*                              Out: the weights of the particles of the posterior distribution.
*                              It is required that <i>|W| = |X|</i>, and that the weights are normalized, i.e. 
*                              <i>sum(W) = 1</i>.
*  @param       z       The measurement that is incorporated.
*  @param       h       A pointer to the measurement function of the form <i>z = h(x) + n, n ~ N(0, N)</i>, that 
*                       is used to perform the control update step.
*  @param       N       The measurement noise covariance matrix of the measurement function of the form 
*                       <i>z = h(x) + n, n ~ N(0, N)</i> that is used to perform the measurement update step.
*  \ingroup pf
*/

inline void pfMeasurementUpdate(std::vector<Matrix>& X, std::vector<double>& W, const Matrix& z, Matrix (*h)(const Matrix&), const Matrix& N) 
{
	size_t zDim = z.numRows();

	double constant = -0.5*zDim*log(2*M_PI) - 0.5*log(det(N)); // O(zDim^3)
	Matrix Ninv = !N;
	double maxLogW = log(0.0);
	for (size_t i = 0; i < X.size(); ++i) {
		Matrix zHat = z - h(X[i]);
		W[i] = log(W[i]) - 0.5*tr(~zHat*Ninv*zHat) + constant; // O(|X| zDim^2)
		if (W[i] > maxLogW) {
			maxLogW = W[i];
		}
	}

	// scale W[i] such that exp(maxLogW) == 1 for numerical stability
	double totalW = 0;
	for (size_t i = 0; i < X.size(); ++i) {
		W[i] = exp(W[i] - maxLogW);
		totalW += W[i];
	}

	// normalize weights
	for (size_t i = 0; i < X.size(); ++i) {
		W[i] /= totalW;
	}
}

/*!
*  @brief       Resamples the set of particles using low-variance sequential resampling.
*  @tparam      xDim    The dimension of the state.
*  @param[in,out]       X       In: the set of particles defining the distribution of the state. 
*                              Out: the resampled set of particles defining the distribution of the state.
*  @param[in,out]       W       In: the weights of the particles of the distribution. 
*                              Out: the weights of the resampled particles of the distribution.
*                              It is required that <i>|W| = |X|</i>, and that the weights are normalized, i.e. 
*                              <i>sum(W) = 1</i>.
*  \ingroup pf
*/
inline void resample(std::vector<Matrix>& X, std::vector<double>& W) 
{
	size_t xDim = X[0].numRows();
	std::vector<Matrix> Xnew(X.size());

	double avW = 1.0 / X.size();
	double r = mrandom() * avW;
	size_t i = 0;
	double c = W[0];
	for (size_t j = 0; j < X.size(); ++j) {
		double U = r + j*avW;
		while (U > c) {
			++i;
			c += W[i];
		}
		Xnew[j] = X[i];
	}
	X.swap(Xnew);
	W.assign(W.size(), avW);
}


/*!
*  @brief       Performs a full step of the standard Particle Filter. 
*  @tparam      xDim    The dimension of the state.
*  @tparam      uDim    The dimension of the control input.
*  @tparam      mDim    The dimension of the motion noise.
*  @tparam      zDim    The dimension of the measurement.
*  @tparam      nDim    The dimension of the measurement noise.
*  @param[in,out]       X       In: the set of particles defining the prior distribution of the state. 
*                              Out: the set of particles defining the posterior distribution of the state.
*  @param[in,out]       W       In: the weights of the particles of the prior distribution. 
*                              Out: the weights of the particles of the posterior distribution.
*                              It is required that <i>|W| = |X|</i>, and that the weights are normalized, i.e. 
*                              <i>sum(W) = 1</i>. 
*  @param       u       The control input that is applied.
*  @param       z       The measurement that is incorporated.
*  @param       f       A pointer to the dynamics function of the form <i>x = f(x,u,m), m ~ N(0,I)</i>, that is 
*                       used to perform the control update step.
*  @param       h       A pointer to the measurement function of the form <i>z = h(x,n), n ~ N(0,I)</i>, that is 
*                       used to perform the measurement update step. For the particle filter to function correctly, 
*                       <i>h</i> must be linear in the measurement noise n, i.e. of the form <i>h(x, n) = h(x) 
*                       + N(x)*n</i>,  where <i>N(x)</i> is a matrix that may be an arbitrarily non-linear 
*                       function of <i>x</i>. If the measurement function is not linear in <i>n</i>, it is 
*                       approximated by linearizing <i>h</i> in <i>n = 0</i>.
*  @note        This function is equivalent to sequentially calling pfControlUpdate, pfMeasurementUpdate, and 
*               resample.
*  \ingroup pf
*/
inline void particleFilter(std::vector<Matrix>& X, std::vector<double>& W, const Matrix& u, const Matrix& z, 
						   size_t mDim, Matrix (*f)(const Matrix&, const Matrix&, const Matrix&), 
						   size_t nDim, Matrix (*h)(const Matrix&, const Matrix&)) 
{
	pfControlUpdate(X, u, mDim, f);
	pfMeasurementUpdate(X, W, z, nDim, h);
	resample(X, W);
}

/*!
*  @brief       Performs a full step of the standard Particle Filter. 
*  @tparam      xDim    The dimension of the state.
*  @tparam      uDim    The dimension of the control input.
*  @tparam      mDim    The dimension of the motion noise.
*  @tparam      zDim    The dimension of the measurement.
*  @param[in,out]       X       In: the set of particles defining the prior distribution of the state. 
*                              Out: the set of particles defining the posterior distribution of the state.
*  @param[in,out]       W       In: the weights of the particles of the prior distribution. 
*                              Out: the weights of the particles of the posterior distribution.
*                              It is required that <i>|W| = |X|</i>, and that the weights are normalized, i.e. 
*                              <i>sum(W) = 1</i>. 
*  @param       u       The control input that is applied.
*  @param       z       The measurement that is incorporated.
*  @param       f       A pointer to the dynamics function of the form <i>x = f(x,u,m), m ~ N(0,I)</i>, that is 
*                       used to perform the control update step.
*  @param       h       A pointer to the measurement function of the form <i>z = h(x) + n, n ~ N(0, N)</i>, that 
*                       is used to perform the measurement update step.
*  @param       N       The measurement noise covariance matrix of the measurement function of the form 
*                       <i>z = h(x) + n, n ~ N(0, N)</i> that is used to perform the measurement update step.
*  @note        This function is equivalent to sequentially calling pfControlUpdate, pfMeasurementUpdate, and 
*               resample.
*  \ingroup pf
*/
inline void particleFilter(std::vector<Matrix>& X, std::vector<double>& W, const Matrix& u, const Matrix& z, 
						   size_t mDim, Matrix (*f)(const Matrix&, const Matrix&, const Matrix&), Matrix (*h)(const Matrix&), const Matrix& N) 
{
	pfControlUpdate(X, u, mDim, f);
	pfMeasurementUpdate(X, W, z, h, N);
	resample(X, W);
}


/*!
*  @brief       Performs a control update step of the Unscented Kalman Filter. 
*  @tparam      xDim    The dimension of the state.
*  @tparam      uDim    The dimension of the control input.
*  @tparam      mDim    The dimension of the motion noise.
*  @param[in,out]       xHat   In: the mean of the prior distribution of the state. 
*                              Out: the mean of the posterior distribution of the state.
*  @param[in,out]       Sigma  In: the variance (covariance matrix) of the prior distribution of the state. 
*                              Out: the variance (covariance matrix) of the posterior distribution of the state.
*  @param       u       The control input that is applied.
*  @param       f       A pointer to the dynamics function of the form <i>x = f(x, u, m), m ~ N(0, I)</i>, that 
*                       is used to perform the control update step.
*  @param       alpha   Parameter determining the spread of the sigma points (optional).
*  @param       beta    Parameter to incorporate prior knowledge of the kurtosis of the distribution (optional).
*  @param       kappa   Scaling parameter (optional).
*  \ingroup ukf
*/
inline void ukfControlUpdate(Matrix& xHat, Matrix& Sigma, const Matrix& u, size_t mDim, Matrix (*f)(const Matrix&, const Matrix&, const Matrix&), 
							 double alpha = DEFAULTALPHA, double beta = DEFAULTBETA, double kappa = DEFAULTKAPPA) 
{
	size_t xDim = xHat.numRows();
	size_t uDim = u.numRows();

	size_t L = xDim + mDim;

	double lambda = alpha*alpha*(L + kappa) - L;
	double w = 1 / (2*(L + lambda));
	double mw = lambda / (L + lambda);
	double vw = mw + (1 - alpha*alpha + beta);

	// Control Update

	// propagate sigma points through f
	std::vector<Matrix> X;
	X.push_back(f(xHat, u, zeros(mDim)));

	Matrix V(xDim,xDim), D(xDim,xDim);
	jacobi((L + lambda) * Sigma, V, D);
	for (size_t i = 0; i < xDim; ++i) {
		D(i,i) = sqrt(D(i,i));
	}
	V = V*D; // or V*D*~V ?

	for (size_t i = 0; i < xDim; ++i) {
		X.push_back(f(xHat + V.subMatrix(0, i, xDim, 1), u, zeros(mDim) ));
		X.push_back(f(xHat - V.subMatrix(0, i, xDim, 1), u, zeros(mDim) ));
	}

	Matrix m = zeros(mDim);
	double sigma = sqrt(L + lambda);
	for (size_t i = 0; i < mDim; ++i) {
		m[i] = sigma;
		X.push_back(f(xHat, u, m));
		m[i] = -sigma;
		X.push_back(f(xHat, u, m));
		m[i] = 0;
	}

	// calculate mean
	xHat = mw * X[0];
	for (size_t i = 1; i < X.size(); ++i) {
		xHat += w * X[i];
	}

	// calculate variance
	Sigma = vw * (X[0] - xHat)*~(X[0] - xHat);
	for (size_t i = 1; i < X.size(); ++i) {
		Sigma += w * (X[i] - xHat)*~(X[i] - xHat);
	}
}

/*!
*  @brief       Performs a measurement update step of the Unscented Kalman Filter. 
*  @tparam      xDim    The dimension of the state.
*  @tparam      zDim    The dimension of the measurement.
*  @tparam      nDim    The dimension of the measurement noise.
*  @param[in,out]       xHat   In: the mean of the prior distribution of the state. 
*                              Out: the mean of the posterior distribution of the state.
*  @param[in,out]       Sigma  In: the variance (covariance matrix) of the prior distribution of the state. 
*                              Out: the variance (covariance matrix) of the posterior distribution of the state.
*  @param       z       The measurement that is incorporated.
*  @param       h       A pointer to the measurement function of the form <i>z = h(x, n), n ~ N(0, I)</i>, that 
*                       is used to perform the measurement update step.
*  @param       alpha   Parameter determining the spread of the sigma points (optional).
*  @param       beta    Parameter to incorporate prior knowledge of the kurtosis of the distribution (optional).
*  @param       kappa   Scaling parameter (optional).
*  \ingroup ukf
*/
inline void ukfMeasurementUpdate(Matrix& xHat, Matrix& Sigma, const Matrix& z, size_t nDim, 
								 Matrix (*h)(const Matrix&, const Matrix&), double alpha = DEFAULTALPHA, double beta = DEFAULTBETA, double kappa = DEFAULTKAPPA) 
{
	size_t xDim = xHat.numRows();
	size_t zDim = z.numRows();

	size_t L = xDim + nDim;

	double lambda = alpha*alpha*(L + kappa) - L;
	double w = 1 / (2*(L + lambda));
	double mw = lambda / (L + lambda);
	double vw = mw + (1 - alpha*alpha + beta);

	std::vector<Matrix> Z;
	std::vector<Matrix> X;
	X.push_back(xHat);
	Z.push_back(h(xHat, zeros(nDim)));

	Matrix V(xDim,xDim), D(xDim,xDim);
	jacobi((L + lambda) * Sigma, V, D);
	for (size_t i = 0; i < xDim; ++i) {
		D(i,i) = sqrt(D(i,i));
	}
	V = V*D;

	for (size_t i = 0; i < xDim; ++i) {
		X.push_back(xHat + V.subMatrix(0, i, xDim, 1));
		Z.push_back(h(xHat + V.subMatrix(0, i, xDim, 1), zeros(nDim) ));
		X.push_back(xHat - V.subMatrix(0, i, xDim, 1));
		Z.push_back(h(xHat - V.subMatrix(0, i, xDim, 1), zeros(nDim) ));
	}

	Matrix n = zeros(nDim);
	double sigma = sqrt(L + lambda);
	for (size_t i = 0; i < nDim; ++i) {
		n[i] = sigma;
		Z.push_back(h(xHat, n));
		n[i] = -sigma;
		Z.push_back(h(xHat, n));
		n[i] = 0;
	}

	// calculate mean
	Matrix zHat(zDim);
	zHat = mw * Z[0];
	for (size_t i = 1; i < Z.size(); ++i) {
		zHat += w * Z[i];
	}

	// calculate variance
	Matrix Pzz = vw * (Z[0] - zHat)*~(Z[0] - zHat);
	for (size_t i = 1; i < Z.size(); ++i) {
		Pzz += w * (Z[i] - zHat)*~(Z[i] - zHat);
	}

	// calculate cross-covariance
	Matrix Pxz = vw * (X[0] - xHat)*~(Z[0] - zHat);
	for (size_t i = 1; i < X.size(); ++i) {
		Pxz += w * (X[i] - xHat)*~(Z[i] - zHat);
	}
	for (size_t i = X.size(); i < Z.size(); ++i) {
		Pxz += w * (X[0] - xHat)*~(Z[i] - zHat);
	}

	Matrix K = Pxz / Pzz;
	xHat += K*(z - zHat);
	Sigma -= Pxz*~K;
}

/*!
*  @brief       Performs a full step of the Unscented Kalman Filter. 
*  @tparam      xDim    The dimension of the state.
*  @tparam      uDim    The dimension of the control input.
*  @tparam      mDim    The dimension of the motion noise.
*  @tparam      zDim    The dimension of the measurement.
*  @tparam      nDim    The dimension of the measurement noise.
*  @param[in,out]       xHat   In: the mean of the prior distribution of the state. 
*                              Out: the mean of the posterior distribution of the state.
*  @param[in,out]       Sigma  In: the variance (covariance matrix) of the prior distribution of the state. 
*                              Out: the variance (covariance matrix) of the posterior distribution of the state.
*  @param       u       The control input that is applied.
*  @param       z       The measurement that is incorporated.
*  @param       f       A pointer to the dynamics function of the form <i>x = f(x, u, m), m ~ N(0, I)</i>, that 
*                       is used to perform the control update step.
*  @param       h       A pointer to the measurement function of the form <i>z = h(x, n), n ~ N(0, I)</i>, that 
*                       is used to perform the measurement update step.
*  @param       alpha   Parameter determining the spread of the sigma points (optional).
*  @param       beta    Parameter to incorporate prior knowledge of the kurtosis of the distribution (optional).
*  @param       kappa   Scaling parameter (optional).
*  @note        Performing a full step of the Unscented Kalman Filter is not equivalent to sequentially performing 
*               a control and a measurement update step, as the sigma points are not resampled after the control 
*               update.
*  \ingroup ukf
*/
inline void unscentedKalmanFilter(Matrix& xHat, Matrix& Sigma, const Matrix& u, const Matrix& z, 
								  size_t mDim, Matrix (*f)(const Matrix&, const Matrix&, const Matrix&), 
								  size_t nDim, Matrix (*h)(const Matrix&, const Matrix&), double alpha = DEFAULTALPHA, double beta = DEFAULTBETA, double kappa = DEFAULTKAPPA) 
{
	size_t xDim = xHat.numRows();
	size_t zDim = z.numRows();
	size_t uDim = u.numRows();

	size_t L = xDim + mDim + nDim;
	
	double lambda = alpha*alpha*(L + kappa) - L;
	double w = 1 / (2*(L + lambda));
	double mw = lambda / (L + lambda);
	double vw = mw + (1 - alpha*alpha + beta);

	// Control Update -- O(xDim^3)

	// propagate sigma points through f 
	std::vector<Matrix> X;
	X.push_back(f(xHat, u, zeros(mDim))); 

	Matrix V(xDim,xDim), D(xDim,xDim); // O(xDim^3)
	jacobi((L + lambda) * Sigma, V, D);
	for (size_t i = 0; i < xDim; ++i) {
		D(i,i) = sqrt(D(i,i));
	}
	V = V*D; // or V*D*~V ?

	for (size_t i = 0; i < xDim; ++i) { // O(xDim^2)
		X.push_back(f(xHat + V.subMatrix(0, i, xDim, 1), u, zeros(mDim) ));
		X.push_back(f(xHat - V.subMatrix(0, i, xDim, 1), u, zeros(mDim) ));
	}

	Matrix m = zeros(mDim);
	double sigma = sqrt(L + lambda);
	for (size_t i = 0; i < mDim; ++i) {
		m[i] = sigma;
		X.push_back(f(xHat, u, m));
		m[i] = -sigma;
		X.push_back(f(xHat, u, m));
		m[i] = 0;
	}

	// calculate mean -- O(xDim^2)
	xHat = (mw + 2*nDim*w) * X[0];
	for (size_t i = 1; i < X.size(); ++i) {
		xHat += w * X[i];
	}

	// calculate variance -- O(xDim^3)
	Sigma = (vw + 2*nDim*w) * (X[0] - xHat)*~(X[0] - xHat);
	for (size_t i = 1; i < X.size(); ++i) {
		Sigma += w * (X[i] - xHat)*~(X[i] - xHat);
	}

	// Measurement Update

	// propagate sigma points through h -- O(xDim * zDim)
	std::vector<Matrix> Z;
	for (size_t i = 0; i < X.size(); ++i) {
		Z.push_back(h(X[i], zeros(nDim)));
	}

	Matrix n = zeros(nDim); // O(zDim^2)
	for (size_t i = 0; i < nDim; ++i) {
		n[i] = sigma;
		Z.push_back(h(X[0], n));
		n[i] = -sigma;
		Z.push_back(h(X[0], n));
		n[i] = 0;
	}

	// calculate mean -- O(xDim*zDim + zDim^2)
	Matrix zHat(zDim);
	zHat = mw * Z[0];
	for (size_t i = 1; i < Z.size(); ++i) {
		zHat += w * Z[i];
	}

	// calculate variance -- O(zDim^3 + xDim*zDim^2)
	Matrix Pzz = vw * (Z[0] - zHat)*~(Z[0] - zHat);
	for (size_t i = 1; i < Z.size(); ++i) {
		Pzz += w * (Z[i] - zHat)*~(Z[i] - zHat);
	}

	// calculate cross-covariance -- O(xDim^2*zDim + xDim*zDim^2)
	Matrix Pxz = vw * (X[0] - xHat)*~(Z[0] - zHat);
	for (size_t i = 1; i < X.size(); ++i) {
		Pxz += w * (X[i] - xHat)*~(Z[i] - zHat);
	}
	for (size_t i = X.size(); i < Z.size(); ++i) {
		Pxz += w * (X[0] - xHat)*~(Z[i] - zHat);
	}

	Matrix K = Pxz / Pzz; // O(zDim^2*xDim + zDim^3)
	xHat += K*(z - zHat); // O(xDim*zDim)
	Sigma -= Pxz*~K; // O(xDim^2*zDim)
}



/*!
*  @brief       Performs a control update step of the Extended Kalman Filter. 
*  @tparam      xDim    The dimension of the state.
*  @tparam      uDim    The dimension of the control input.
*  @tparam      mDim    The dimension of the motion noise.
*  @param[in,out]       xHat   In: the mean of the prior distribution of the state. 
*                              Out: the mean of the posterior distribution of the state.
*  @param[in,out]       Sigma  In: the variance (covariance matrix) of the prior distribution of the state. 
*                              Out: the variance (covariance matrix) of the posterior distribution of the state.
*  @param       u       The control input that is applied.
*  @param       f       A pointer to the dynamics function of the form <i>x = f(x, u, m), m ~ N(0, I)</i>, that 
*                       is used to perform the control update step.
*  @param       jStep   Parameter determining the step size used for numerical differentiation (optional).
*  \ingroup ekf
*/
inline void ekfControlUpdate(Matrix& xHat, Matrix& Sigma, const Matrix& u, size_t mDim, 
							 Matrix (*f)(const Matrix&, const Matrix&, const Matrix&), double jStep = DEFAULTSTEPSIZE) 
{
	size_t xDim = xHat.numRows();

	Matrix A = jacobian1(xHat, u, zeros(mDim), xDim, f, jStep); // O(xDim^2)
	Matrix B = jacobian2(xHat, u, zeros(mDim), xDim, f, jStep); // O(xDim^2)
	Matrix M = jacobian3(xHat, u, zeros(mDim), xDim, f, jStep); // O(xDim^2)

	xHat = f(xHat, u, zeros(mDim)); // O(xDim)
	Sigma = A*Sigma*~A + M*~M;        // O(xDim^3)
}

/*!
*  @brief       Performs a control update step of the Extended Kalman Filter. 
*  @tparam      xDim    The dimension of the state.
*  @tparam      uDim    The dimension of the control input.
*  @param[in,out]       xHat   In: the mean of the prior distribution of the state. 
*                              Out: the mean of the posterior distribution of the state.
*  @param[in,out]       Sigma  In: the variance (covariance matrix) of the prior distribution of the state. 
*                              Out: the variance (covariance matrix) of the posterior distribution of the state.
*  @param       u       The control input that is applied.
*  @param       f       A pointer to the dynamics function of the form <i>x = f(x, u) + m, m ~ N(0, M)</i>, that 
*                       is used to perform the control update step.
*  @param       M       The motion noise covariance matrix of the dynamics function of the form 
*                       <i>x = f(x, u) + m, m ~ N(0, M)</i> that is used to perform the control update step.
*  @param       jStep   Parameter determining the step size used for numerical differentiation (optional).
*  \ingroup ekf
*/
inline void ekfControlUpdate(Matrix& xHat, Matrix& Sigma, const Matrix& u, size_t mDim,  
							 Matrix (*f)(const Matrix&, const Matrix&), const Matrix& M, double jStep = DEFAULTSTEPSIZE) 
{
	size_t xDim = xHat.numRows();

	Matrix A = jacobian1(xHat, u, xDim, f, jStep); // O(xDim^2)
	Matrix B = jacobian2(xHat, u, xDim, f, jStep); // O(xDim^2)

	xHat = f(xHat, u); // O(xDim)
	Sigma = A*Sigma*~A + M;        // O(xDim^3)
}

/*!
*  @brief       Performs a measurement update step of the Extended Kalman Filter. 
*  @tparam      xDim    The dimension of the state.
*  @tparam      zDim    The dimension of the measurement.
*  @tparam      nDim    The dimension of the measurement noise.
*  @param[in,out]       xHat   In: the mean of the prior distribution of the state. 
*                              Out: the mean of the posterior distribution of the state.
*  @param[in,out]       Sigma  In: the variance (covariance matrix) of the prior distribution of the state. 
*                              Out: the variance (covariance matrix) of the posterior distribution of the state.
*  @param       z       The measurement that is incorporated.
*  @param       h       A pointer to the measurement function of the form <i>z = h(x, n), n ~ N(0, I)</i>, that 
*                       is used to perform the measurement update step.
*  @param       jStep   Parameter determining the step size used for numerical differentiation (optional).
*  \ingroup ekf
*/
inline void ekfMeasurementUpdate(Matrix& xHat, Matrix& Sigma, const Matrix& z, size_t nDim,
								 Matrix (*h)(const Matrix&, const Matrix&), double jStep = DEFAULTSTEPSIZE) 
{
	size_t zDim = z.numRows();

	Matrix H = jacobian1(xHat, zeros(nDim), zDim, h, jStep); // O(zDim*xDim)
	Matrix N = jacobian2(xHat, zeros(nDim), zDim, h, jStep); // O(zDim^2)

	Matrix K = Sigma*~H/(H*Sigma*~H + N*~N); // O(zDim*xDim^2 + zDim^2*xDim + zDim^3)

	xHat += K*(z - h(xHat, zeros(nDim))); // O(xDim*zDim)
	Sigma -= K*(H*Sigma);                   // O(xDim^2*zDim)
}

/*!
*  @brief       Performs a measurement update step of the Extended Kalman Filter. 
*  @tparam      xDim    The dimension of the state.
*  @tparam      zDim    The dimension of the measurement.
*  @param[in,out]       xHat   In: the mean of the prior distribution of the state. 
*                              Out: the mean of the posterior distribution of the state.
*  @param[in,out]       Sigma  In: the variance (covariance matrix) of the prior distribution of the state. 
*                              Out: the variance (covariance matrix) of the posterior distribution of the state.
*  @param       z       The measurement that is incorporated.
*  @param       h       A pointer to the measurement function of the form <i>z = h(x) + n, n ~ N(0, N)</i>, that 
*                       is used to perform the measurement update step.
*  @param       N       The measurement noise covariance matrix of the measurement function of the form 
*                       <i>z = h(x) + n, n ~ N(0, N)</i> that is used to perform the measurement update step.
*  @param       jStep   Parameter determining the step size used for numerical differentiation (optional).
*  \ingroup ekf
*/
inline void ekfMeasurementUpdate(Matrix& xHat, Matrix& Sigma, const Matrix& z, 
								 Matrix (*h)(const Matrix&), const Matrix& N, double jStep = DEFAULTSTEPSIZE) 
{
	size_t zDim = z.numRows();

	Matrix H = jacobian1(xHat, zDim, h, jStep); // O(zDim*xDim)

	Matrix K = Sigma*~H/(H*Sigma*~H + N); // O(zDim*xDim^2 + zDim^2*xDim + zDim^3)

	xHat += K*(z - h(xHat)); // O(xDim*zDim)
	Sigma -= K*(H*Sigma);                   // O(xDim^2*zDim)
}

/*!
*  @brief       Performs a full step of the Extended Kalman Filter. 
*  @tparam      xDim    The dimension of the state.
*  @tparam      uDim    The dimension of the control input.
*  @tparam      mDim    The dimension of the motion noise.
*  @tparam      zDim    The dimension of the measurement.
*  @tparam      nDim    The dimension of the measurement noise.
*  @param[in,out]       xHat   In: the mean of the prior distribution of the state. 
*                              Out: the mean of the posterior distribution of the state.
*  @param[in,out]       Sigma  In: the variance (covariance matrix) of the prior distribution of the state. 
*                              Out: the variance (covariance matrix) of the posterior distribution of the state.
*  @param       u       The control input that is applied.
*  @param       z       The measurement that is incorporated.
*  @param       f       A pointer to the dynamics function of the form <i>x = f(x, u, m), m ~ N(0, I)</i>, that 
*                       is used to perform the control update step.
*  @param       h       A pointer to the measurement function of the form <i>z = h(x, n), n ~ N(0, I)</i>, that 
*                       is used to perform the measurement update step.
*  @param       jStep   Parameter determining the step size used for numerical differentiation (optional).
*  @note        Performing a full step of the Extended Kalman Filter is equivalent to sequentially performing 
*               a control and a measurement update step.
*  \ingroup ekf
*/
inline void extendedKalmanFilter(Matrix& xHat, Matrix& Sigma, const Matrix& u, const Matrix& z, 
								 size_t mDim, Matrix (*f)(const Matrix&, const Matrix&, const Matrix&), 
								 size_t nDim, Matrix (*h)(const Matrix&, const Matrix&), double jStep = DEFAULTSTEPSIZE) {
	ekfControlUpdate(xHat, Sigma, u, mDim, f, jStep);
	ekfMeasurementUpdate(xHat, Sigma, z, nDim, h, jStep);
}


/*!
*  @brief       Performs a control update step of the Kalman Filter. 
*  @tparam      xDim    The dimension of the state.
*  @tparam      uDim    The dimension of the control input.
*  @param[in,out]       xHat   In: the mean of the prior distribution of the state. 
*                              Out: the mean of the posterior distribution of the state.
*  @param[in,out]       Sigma  In: the variance (covariance matrix) of the prior distribution of the state. 
*                              Out: the variance (covariance matrix) of the posterior distribution of the state.
*  @param       u       The control input that is applied.
*  @param       A       Matrix of the linear dynamics function of the form <i>x = Ax + Bu + m, m ~ N(0, M)</i>, 
*                       that is used to perform the control update step.
*  @param       B       Matrix of the linear dynamics function of the form <i>x = Ax + Bu + m, m ~ N(0, M)</i>, 
*                       that is used to perform the control update step.
*  @param       M       Motion noise covariance matrix of the linear dynamics function of the form <i>x = Ax + Bu 
*                       + m, m ~ N(0, M)</i>, that is used to perform the control update step.
*  \ingroup kf
*/
inline void kfControlUpdate(Matrix& xHat, Matrix& Sigma, const Matrix& u, const Matrix& A, const Matrix& B, const Matrix& M) 
{
	xHat = A*xHat + B*u; // O(xDim)
	Sigma = A*Sigma*~A + M;        // O(xDim^3)
}


/*!
*  @brief       Performs a measurement update step of the Kalman Filter. 
*  @tparam      xDim    The dimension of the state.
*  @tparam      zDim    The dimension of the measurement.
*  @param[in,out]       xHat   In: the mean of the prior distribution of the state. 
*                              Out: the mean of the posterior distribution of the state.
*  @param[in,out]       Sigma  In: the variance (covariance matrix) of the prior distribution of the state. 
*                              Out: the variance (covariance matrix) of the posterior distribution of the state.
*  @param       z       The measurement that is incorporated.
*  @param       H       Matrix of the linear measurement function of the form <i>z = Hx + n, n ~ N(0, N)</i>, 
*                       that is used to perform the measurement update step.
*  @param       N       Measurement noise covariance matrix of the linear measurement function of the form 
*                       <i>z = Hx + n, n ~ N(0, N)</i>, that is used to perform the measurement update step.
*  \ingroup kf
*/
inline void kfMeasurementUpdate(Matrix& xHat, Matrix& Sigma, const Matrix& z, const Matrix& H, const Matrix& N) 
{
	Matrix K = Sigma*~H/(H*Sigma*~H + N); // O(zDim*xDim^2 + zDim^2*xDim + zDim^3)

	xHat += K*(z - H*xHat); // O(xDim*zDim)
	Sigma -= K*(H*Sigma);   // O(xDim^2*zDim)
}

/*!
*  @brief       Performs a full step of the Kalman Filter. 
*  @tparam      xDim    The dimension of the state.
*  @tparam      uDim    The dimension of the control input.
*  @tparam      zDim    The dimension of the measurement.
*  @param[in,out]       xHat   In: the mean of the prior distribution of the state. 
*                              Out: the mean of the posterior distribution of the state.
*  @param[in,out]       Sigma  In: the variance (covariance matrix) of the prior distribution of the state. 
*                              Out: the variance (covariance matrix) of the posterior distribution of the state.
*  @param       u       The control input that is applied.
*  @param       z       The measurement that is incorporated.
*  @param       A       Matrix of the linear dynamics function of the form <i>x = Ax + Bu + m, m ~ N(0, M)</i>, 
*                       that is used to perform the control update step.
*  @param       B       Matrix of the linear dynamics function of the form <i>x = Ax + Bu + m, m ~ N(0, M)</i>, 
*                       that is used to perform the control update step.
*  @param       M       Motion noise covariance matrix of the linear dynamics function of the form <i>x = Ax + Bu 
*                       + m, m ~ N(0, M)</i>, that is used to perform the control update step.
*  @param       H       Matrix of the linear measurement function of the form <i>z = Hx + n, n ~ N(0, N)</i>, 
*                       that is used to perform the measurement update step.
*  @param       N       Measurement noise covariance matrix of the linear measurement function of the form 
*                       <i>z = Hx + n, n ~ N(0, N)</i>, that is used to perform the measurement update step.
*  @note        Performing a full step of the Kalman Filter is equivalent to sequentially performing 
*               a control and a measurement update step.
*  \ingroup kf
*/
inline void kalmanFilter(Matrix& xHat, Matrix& Sigma, const Matrix& u, const Matrix& z, 
						 const Matrix& A, const Matrix& B, const Matrix& M, const Matrix& H, const Matrix& N) 
{
	kfControlUpdate(xHat, Sigma, u, A, B, M);
	kfMeasurementUpdate(xHat, Sigma, z, H, N);
}

#endif


float entropyMetric(Matrix (*f)(const Matrix&, const Matrix&, const Matrix&), void (*initGuess)(Matrix&, Matrix&), void (*initSim)(double &, int &, int &, std::vector<mPair> *));
