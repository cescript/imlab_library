///@file mlcore.h
#ifndef __IMLAB_MLCORE_H__
#define __IMLAB_MLCORE_H__

// Include C99 libraries
#include <omp.h>
#include <math.h>
// Include IMLAB general headers
#include "core.h"
#include "prcore.h"

/* Define Machine Learning Functions and Useful Structures */
// maximum number of the class in multi class classification
#define IM_MAX_MULTI_CLASS 100

/*
#define LSRL1 0
#define LSRL2 1
#define SVRL1 2
#define SVRL2 3

// regression models (x < 5 )
#define IM_CLASSORREG 4.5
// classification models ( x > 5)

#define LREL1 6
#define LREL2 7
#define SVML1 8
#define SVML2 9
*/



/**
@struct glm_t
@brief glm_t struct keeps the given GLM parameters in a single variable and computes the necessary parameters at the construction step.

Generalized Linear Models are machine learning techniques that approximates the output labels \f$y\f$ as the following linear form.
\f[y = w \times x + b\f] Here \f$w\f$ is the linear coefficient and \f$b\f$ is the bias.
The problem of the GLM are to find the best coefficient vector and bias value depending on the given restrictions on these vectors.
In the most generalized way the cost function can be defined as the sum of the classification and regularization loss
\f[C(w,b) := \frac{1}{n} \sum_i { L( y_i, a_i = w_i \times x_i+b) } + R( w, \lambda )\f]

Using the above cost function one can create different sets of problems/solutions by simply changing the lost function \f$L\f$ and regularization function \f$R\f$.
Here a list of supported lost and regularization functions are given.

 <table>
 <tr><th>algorithm<th>lost function \f$(L)\f$<th>regularization \f$(R)\f$
 <tr><td>Least-squares without regularization<td>\f$L = (a - y)^2\f$<td>\f$R = 0\f$
 <tr><td>Least-squares using a ridge (L1) penalty<td>\f$L = (a - y)^2\f$<td>\f$R = \lambda |w|\f$
 <tr><td>Least-squares using a lasso (L2) penalty<td>\f$L = (a - y)^2\f$<td>\f$R = \lambda w^2/2\f$
 <tr><td colspan="3">
 <tr><td>Logistic regression without regularization<td>\f$L = \log(1+\exp(-a y))\f$<td>\f$R = 0\f$
 <tr><td>Logistic regression using a L1 penalty<td>\f$L = \log(1+\exp(-a y))\f$<td>\f$R = \lambda |w|\f$
 <tr><td>Logistic regression using a L2 penalty<td>\f$L = \log(1+\exp(-a y))\f$<td>\f$R = \lambda w^2/2\f$
 <tr><td colspan="3">
 <tr><td>Support Vector Machine without regularization<td>\f$L = \max(0, 1-a y)\f$<td>\f$R = 0\f$
 <tr><td>Support Vector Machine using a L1 penalty<td>\f$L = \max(0, 1-a y)\f$<td>\f$R = \lambda |w|\f$
 <tr><td>Support Vector Machine using a L2 penalty<td>\f$L = \max(0, 1-a y)\f$<td>\f$R = \lambda w^2/2\f$
 </table>

### Supported Algorithms ###
#### Logistic Regression ####
Logistic regression is a special case of generalized linear models. These models assume that the data points are generated from a non-linear transformation of a linear function.
If we call this non-linear function as \f$y = \theta(s)\f$, \f$s\f$ will be the output of the linear function
\f[s=w^{T} x\f] where \f$x_n \in{R^d}\f$ correspond to input vector with \f$d\f$ dimensions and \f$y_n \in{\{-1,+1\}}\f$ is the
associated label for the data vector \f$x_n\f$. Here if the \f$\theta(s)\f$ is selected as the
\f[\theta(s) = s\f] the problem turns into the liner regression where the output has no bounds and for the
\f[\theta(s) = sign(s)\f] the model becomes the linear classifier where the outputs are \f$-1\f$ and \f$1\f$.

For the logistic regression, the non-linear function is selected as \f[\theta(s) = \frac{e^s}{1+e^s}\f] which we can pretend to outputs
as the probability values. Here we can express the above equation in terms of probabilities as
\f[P(y=1 | x)= \theta(s) = \frac{1}{1+e^{-w^{T} x}}\f] and \f[P(y=-1 | x)= 1-\theta(s) = \frac{e^{-w^{T} x}}{1+e^{-w^{T} x}}=\frac{1}{1+e^{w^{T} x}}\f]

Note that we derive the second equation using the fact that the sum of two probabilities \f$P(y=1 | x)\f$ and \f$P(y=0 | x)\f$ must be one.
We can further simplify the likelihood using the fact that \f$\theta(s)=1-\theta(-s)\f$, so the probabilities becomes
\f[P(y|x) = \frac{1}{1+e^{-yw^{T} x}}\f]
Here we try to find such weights \f$w\f$ which maximizes these probabilities for all samples \f$n=1,2,\dots,N\f$.
The problem can be expressed in terms of likelihood function as \f[L(x,y|w) = \Pi_{n=1}^N P(y|x) = \Pi_{n=1}^N \frac{1}{1+e^{-yw^{T} x}}\f]

where we try to find \f$w\f$ which maximizes \f$L(x,y|w)\f$. Since the likelihood function involves exponentials, using log-likelihood of the data create more easy-to-solve equation.
In this case the log-likelihood function becomes
\f[L(x,y|w) = \sum_{n=1}^N \log\left (\frac{1}{1+e^{-yw^{T} x}}\right )\f] and maximization of this log-likelihood function is equivalent to minimize
\f[E(x,y|w) = \sum_{n=1}^N \log\left ({1+e^{-yw^{T} x}}\right )\f]

Note that the resulting error function has not a closed-form which the solution need be found using an iterative algorithm. Moreover, the penalty for the large weights must also be considered to avoid over fitting to data.
In this thesis, following \cite{towards to automated caricature}, $L_2$ regularized logistic regression is used \cite{Dual Coordinate Descent Methods for Logistic Regression}, so the objective function become

\f[\underset{w}{\arg\min} \sum_{n=1}^N \log\left ({1+e^{-yw^{T} x}}\right ) + \frac{\lambda}{2} \sum_{n=1}^{N} w_n^2\f]
where the \f$\lambda\f$ is the trade off between matching the training data and the generalization. To solve the above equation we use the gradient descent method
\f[w_k^{(t+1)}=w_k^{(t)}-\epsilon \frac{\partial E}{\partial w}\f]
where the gradient is \f[\frac{\partial E}{\partial w} = -\sum_{n=1}^N yx\log\left ({1+e^{-yw^{T} x}}\right ) + \lambda \sum_{n=1}^{N} w_n\f]


#### Support Vector Machine (SVM) ####
Support Vector Machine (SVM) is a successful learning algorithm that has many uses in pattern recognition for classification and regression \cite{cortes1995support,moghaddam2000gender}.
The basic idea behind the SVM is finding a hyper-plane or hyper-planes which separate the two classes with a maximum margin, transforming the problem into a quadratic optimization problem.
For the problems that cannot be linearly separated in the input space, SVM maps the input space into a much higher-dimensional feature space, where separation is possible.
The power of SVM stems from the principle of structural risk minimization that while increasing the classification success, keeps down the VC dimension as much as possible,
so that the generalization of the data is maintained. In other words, classification by mapping the data into high-dimensional space and
maximizing margin there is the proof of the founded hyper-plane is the best descriptor for that data rather than over-fitting (see Fig. \ref{fig:svm}).

The basic mechanism of SVM may be stated as follows:
Given a labeled set of N samples \f$(x_n,y_n)\f$ where \f$x_n \in{R^d}\f$ correspond to input vectors and \f$y_n \in{\{-1,+1\}}\f$ associated label for current vector \cite{parsons2005introduction}. A linear classification problem may be expressed in terms of our notation as follows:

\f[f(\mathbf{x})=\mathbf{w}^{T} \phi(\mathbf{x})+b\f]

where \f$\phi(\mathbf{x})\f$ denotes to the feature-space transformation function that makes the data vector \f$\mathbf{x}\f$ linearly separable in transform domain, \f$\mathbf{w}\f$ is the weights for the data in feature space and \f$b\f$ is the bias term. Assuming that the data is linearly separable in feature space, there is at least one \f$\mathbf{w}\f$ and \f$b\f$ that satisfies \f$f(\mathbf{x_n})>0\f$ for \f$y_n=+1\f$ and \f$f(x_n)<0\f$ for \f$y_n=-1\f$, note that \f$y_nf(x_n)>0\f$ for all training points.

There might be of course many hyper-plane created by different \f$\mathbf{w}\f$ and \f$b\f$, SVM aims to find that gives the smallest generalization error by making the hyper-plane as far as possible to the nearest data points also known as support vectors \cite{parsons2005introduction}. The distance of a point \f$x_n\f$ to the hyper-plane which is defined by \f$\mathbf{w}^{T} \phi(\mathbf{x})+b=0\f$ is:

\f[\frac{|f(x_n)|}{\Vert\mathbf{w}\Vert}=\frac{y_n(\mathbf{w}^{T} \phi(x_n)+b)}{\Vert\mathbf{w}\Vert}\f]

Note that \f$|f(\mathbf{x})|\f$ can be replaced by \f$y_nf(\mathbf{x})\f$ under the condition that all the data points are correctly classified.

In order to maximize the distance between hyper-plane and the nearest data point, assuming that the \f$x_n\f$ is the closest point to the plane,we are looking for the arguments

\f[\underset{{\mathbf{w},b}}{\arg\min} \left\{\frac{1}{\Vert \mathbf{w} \Vert} {\min_{n}\left[y_n(\mathbf{w}^{T} \phi(x_n)+b)\right]}\right\}\f]

Because of the complexity of direct solution of the problem, the equation may be expressed as an equivalent problem. Multiplying the distances by a constant in scale domain does not change the problem, so using this as an advantage, we can find a scaling constant that assures:

\f[y_n(\mathbf{w}^{T} \phi(x_n)+b)=1\f]

Note that this scaling also assures that:

\f[y_n(\mathbf{w}^{T} \phi(x_n)+b)\geq 1\f]

In order to solve maximization problem, the problem is quickly transformed into quadratic minimization problem:

\f[\underset{{\mathbf{w},b}}{\arg\min}\frac{1}{2}\mathbf{w}^{T}\mathbf{w}\f]

Note that we still solve the equivalent problem under the constrain that \f$y_n(\mathbf{w}^{T} \phi(x_n)+b)\geq1\f$. One can simply notice that this constrained optimization problem may easily be solved using Lagrange multipliers

\f[L(\mathbf{w},b,\mathbf{a})=\frac{1}{\Vert\mathbf{w}\Vert ^2}-\sum_{n=1}^{N}a_n\left\{y_n(\mathbf{w}^{T} \phi(x_n)+b)-1\right\}\f]

Here \f$a_n\f$ is an non-negative multiplier for each constrain, for the minus sign in front of the \f$a_n\f$, problem can also be considered as an maximization problem with respect to the \f$a_n\f$. In order to find a solution to this quadratic problem, derivation of the equation with respect to \f$\mathbf{w}\f$ and \f$b\f$ must be set to zero.

\f[\frac{\partial{L}}{\partial\mathbf{w}}=\mathbf{w}-\sum_{n=1}^N{a_ny_n\phi(x_n)}\f]

\f[\frac{\partial{L}}{\partial{b}}=-\sum_{n=1}^N{a_ny_n}\f]

Substituting these equations into the original one, we come up with a dual representation problem:

\f[\hat{L}(a)=\sum_{n=1}^N{a_n}-\frac{1}{2}\sum_{n=1}^N\sum_{m=1}^N{a_na_my_ny_m\phi(x_n)^{T} \phi(x_m)}\f]

which maximize the margin with respect to the \f$a_n\f$ under the following constrains:

\f[a_n\geq 0, \quad for\; n=1,2,...N\f]

\f[\sum_{n=1}^N{a_ny_n}=0\f]

Note that the \f$\phi(x)\f$ could be a transform function that represent inputs in an infinite dimension space. However the solution of the problem only needs the dot product of \f$\phi(x_n)^{T} \phi(x_m)\f$, so for the solution of the problem, the direct computation of the high dimension feature space is not necessary. Defining a \f$K(x_n,x_m)\f$ kernel function which consist of dot products of the input vector, the problem could be solved in low complexity. Definition of the \f$\phi(x)\f$ function determines the kernel function, some of the kernel function frequently used with SVM given as follows:

 <table>
 <tr><th>kernel<th>formula
 <tr><td>Linear<td>\f$K(x_n,x_m)=x_n^{T} x_m\f$
 <tr><td>Polynomial<td>\f$K(x_n,x_m)=(x_n^{T} x_m+c)^d\f$
 <tr><td>Radial Basis<td>\f$K(x_n,x_m)=\exp(\frac{\Vert x_n-x_m \Vert_2^2}{\sigma^2}\f$
 </table>

The \f$a_n\geq 0\f$'s coming from the solution shows the support vectors, so the equation of the hyper-plane is defined by:

\f[
\mathbf{w}=\sum_{n\in SV}{a_ny_nx_n}
\f]



 */
struct glm_t;


#define LSRL1 0
#define LSRL2 1
#define SVRL1 2
#define SVRL2 3

// regression models (x < 5 )
#define IM_CLASSORREG 4.5
// classification models ( x > 5)

#define LREL1 6
#define LREL2 7
#define SVML1 8
#define SVML2 9

// for fern train solver options
#define FERN_CUSTOM_REGRESSOR 0
#define FERN_CUSTOM_CLASSIFIER 1
#define FERN_RANDOM_REGRESSOR 2 // random feature selector + regressor
#define FERN_CORRELATION_REGRESSOR 3 // correlation based feature selector + regressor
#define FERN_RANDOM_CLASSIFIER 4 // random feature selector + regressor
#define FERN_CORRELATION_CLASSIFIER 5 // correlation based feature selector + regressor



/// glm_t Functions are included
// "-M 1000 -eta 0.1 -epsilon 0.001 -lambda 0.01"
struct glm_t *glm_create(uint32_t solver, char options[]);
return_t glm_view(struct glm_t *model);
/// HIGH LEVEL FUNCTIONS TO CALL FROM THE IMLAB APPLICATIONS
return_t glm_train(matrix_t *in, vector_t *label, struct glm_t *model);
return_t glm_predict(matrix_t *in, matrix_t *label, struct glm_t *model);
/// glm_t model import/export functions
struct glm_t* glm_read(const char *filename);
return_t glm_write(struct glm_t *net, const char *filename);


// fern regressor
struct fern_t* fern_create(uint8_t solver, char options[]);
// set the feature selection, binarization methods and auxiliary pointer
return_t fern_set(struct fern_t *model, return_t (*select)(float*, float*, float*, void**, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t **, float **, uint32_t), uint32_t (*binarize)(float *data, uint32_t *p, float *t), void *aux);
/// HIGH LEVEL FUNCTIONS TO CALL FROM THE IMLAB APPLICATIONS
return_t fern_train(matrix_t *input, matrix_t *output, struct fern_t *model);
return_t fern_predict(matrix_t *in, matrix_t *output, struct fern_t *model);
// visualize the selected features onto the image
matrix_t* fern_visualize(uint32_t out_width, uint32_t out_height, uint32_t scale, struct fern_t *model);

struct fern_t* fern_read(const char *filename);
return_t fern_write(struct fern_t *model, const char *filename);

#endif