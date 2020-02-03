#include <stdlib.h>
#include "private/iocore_private.h" // for getopt
#include "private/mlcore_private.h"
/*
y = Beta*x + Bias;

The cost function is defined as the sum of the classification and regularization loss

C(w,b) := (1/n) SUM_i { L( yi, ai = wi*xi+b) } + R( w, lambda )

Least-squares without regularization        ------> L = (a - y)^2, lambda = 0, R = lambda;
Least-squares using a ridge (L1) penalty    ------> L = (a - y)^2, lambda > 0, R = lambda*fabs(w);
Least-squares using a lasso (L2) penalty    ------> L = (a - y)^2, lambda > 0, R = lambda*w*w/2;

Logistic regression without regularization: ------> L = log(1+exp(-a*y)), lambda = 0, R = 0;
Logistic regression using a L1 penalty    : ------> L = log(1+exp(-a*y)), lambda > 0, R = lambda*fabs(w);
Logistic regression using a L2 penalty    : ------> L = log(1+exp(-a*y)), lambda > 0, R = lambda*w*w/2;

Support Vector Mac. without regularization: ------> L = max(0, 1-a*y), lambda = 0, R = 0;
Support Vector Mac. using a L1 penalty    : ------> L = max(0, 1-a*y), lambda > 0, R = lambda*fabs(w);
Support Vector Mac. using a L2 penalty    : ------> L = max(0, 1-a*y), lambda > 0, R = lambda*w*w/2;
*/




// creates a default GLM model
struct glm_t *glm_default(uint32_t solver) {
    struct glm_t *model = (struct glm_t*) malloc(sizeof(struct glm_t));
    if(model == NULL) {
        return NULL;
    }
    // fill the default values
    model->options.solver   = solver;
    model->options.max_iter = 1000;
    model->options.eta      = 0.01;
    model->options.epsilon  = 0.001;
    model->options.lambda   = 0.001;
    // initilaize with 0, shows that the model is not trained
    model->nr_feature = 0;
    model->nr_class   = 0;
    model->nr_learner = 0;
    model->cost = (float*)  NULL;
    model->bias = (float*)  NULL;
    model->beta = (float**) NULL;
    // return the created model
    return model;
}

// "-M 1000 -eta 0.1 -epsilon 0.001 -lambda 0.01"
struct glm_t *glm_create(uint32_t solver, char options[]) {

    struct glm_t *model = glm_default(solver);
    if(model == NULL) {
        return NULL;
    }
    // get the given options into the model
    model->options.solver   = solver;
    getopt(uint32_t, options, "max_iter", &model->options.max_iter);
    getopt(float, options, "eta", &model->options.eta);
    getopt(float, options, "epsilon", &model->options.epsilon);
    getopt(float, options, "lambda", &model->options.lambda);
    // return the created model
    return model;
}

return_t glm_view(struct glm_t *model) {

    int i;
    printf("Parameters of the Generalized Linear Model\n");
    printf("Options:\n");
    printf("> Solver : %d\n", model->options.solver);
    printf("> MaxIter: %d\n", model->options.max_iter);
    printf("> Eta    : %3.5f\n", model->options.eta);
    printf("> Epsilon: %3.5f\n", model->options.epsilon);
    printf("> Lambda : %3.5f\n", model->options.lambda);

    // check the model is trained
    if(model->nr_feature == 0) { return SUCCESS; }

    printf("Training results:\n");
    printf("> #Features : %d\n", model->nr_feature);
    printf("> #Class    : %d\n", model->nr_class);
    printf("> #Learner  : %d\n", model->nr_learner);

    return SUCCESS;
}

float lsr_transform(float x) { return x; }
float svr_transform(float x) { return x; }

float svm_transform(float x) { return x > 0 ? +1:-1; }
float lre_transform(float x) { return sigmoid(x) > 0.5 ? +1:-1; }


// gradient loss
float gLSRloss(float a, float y, float x, float e) { return -(y-a*x)*x; }
float gLOGloss(float a, float y, float x, float e) { return -y*sigmoid(-y*a)*x; }
float gSVCloss(float a, float y, float x, float e) { return a*y < 1 ? -y*x:0; }
float gSVRloss(float a, float y, float x, float e) {
    if( (a-y) > e )      { return x; }
    else if( (y-a) > e ) { return -x; }
    return 0;
}

float rL1loss(float w, float l) { return l*( (w > 0.0f) - (w < 0.0f) ); }
float rL2loss(float w, float l) { return l*w; }

/* Stochastic Gradient Descent Algorithm */
void sgd_solve (matrix_t *feature, matrix_t *labels, uint32_t label, float *weight, float *bias,
     // learning parameters
     const int   SGD_SOL,//solver
     const float SGD_EPS,//epsilon
     const float SGD_ETA,//learning coefficient
     const float SGD_LAM,//lambda
     const int   SGD_MAX //max iter
     )
{

    int i,k,f, iter = 0;

    float rho = 0;

    uint32_t sample_size = rows(feature);
    uint32_t feature_size = cols(feature);

    // select label -1 and +1 and do training over them
    uint32_t *idx = random_permutation(sample_size);
    // arrays to hold gradient and regularization loss
    float grho, rrho;
    float *gloss = (float*) malloc(feature_size * sizeof(float));

    // generic gradient loss function
    float (*gXloss)(float,float,float,float);
    float (*rXloss)(float, float);

    // simple if-else, no need to use else (it is small comparison)
    if(SGD_SOL == LSRL1) { gXloss = gLSRloss; rXloss = rL1loss; }
    if(SGD_SOL == LSRL2) { gXloss = gLSRloss; rXloss = rL2loss; }
    if(SGD_SOL == LREL1) { gXloss = gLOGloss; rXloss = rL1loss; }
    if(SGD_SOL == LREL2) { gXloss = gLOGloss; rXloss = rL2loss; }
    if(SGD_SOL == SVML1) { gXloss = gSVCloss; rXloss = rL1loss; }
    if(SGD_SOL == SVML2) { gXloss = gSVCloss; rXloss = rL2loss; }
    if(SGD_SOL == SVRL1) { gXloss = gSVRloss; rXloss = rL1loss; }
    if(SGD_SOL == SVRL2) { gXloss = gSVRloss; rXloss = rL2loss; }

    for(iter=0; iter < SGD_MAX; iter++) 
    {
        /* compute learning rate */
        float eta = SGD_ETA / (1 + SGD_LAM*SGD_ETA*iter);

        // pick a random_people in dataset
        int s = idx[iter%sample_size];

        float *data_s = data(float, feature, s,0);
        float label_s = at(float, labels, s, label);

        // compute the xTx
        float acc = rho;
        for (i = 0 ; i < feature_size; ++i)
         {
            acc += weight[i]*data_s[i];
        }
        // gradient step generic loss function is used
        for (i = 0 ; i < feature_size; ++i) 
        {
            gloss[i] = gXloss(acc,label_s,data_s[i],SGD_EPS);
        }
        grho = gXloss(acc,label_s,1,SGD_EPS);

        // regularization step
        float rloss = 0;
        for (i = 0 ; i < feature_size ; ++i) 
        {
            rloss += rXloss(weight[i], SGD_LAM);
        }
        rloss /= feature_size;
        rrho = rXloss(rho, SGD_LAM);

        // update step
        for (i = 0 ; i < feature_size; ++i) 
        {
            weight[i] -= eta*(gloss[i] + rloss);
        }
        rho -= eta*(grho + rrho);

    }
    *bias = rho;
    // done
    free(idx);
}

//! TODO FIXME: ICERIYE DIKKAT ET
// HIGH LEVEL FUNCTIONS TO CALL FROM THE IMLAB APPLICATIONS
return_t glm_train(matrix_t *input, matrix_t *label, struct glm_t *model) {

    uint32_t i = 0, c = 0;
    uint32_t numberOfClass = cols(label);

    //This function assumes that the given input is either y in [-1,1] or y={-1,1}
    if(rows(input) != rows(label)) {
        message(ERROR, "input and label matrice have different number of samples");
        return ERROR_DIMENSION_MISMATCH;
    }
    if(channels(input) != 1) {
        message(WARNING, "expects its arguments to be 1-dimensional matrice");
    }
    if(!is_numeric(input)) {
        message(ERROR, "input must be a numeric matrix");
        return ERROR_TYPE_MISMATCH;
    }

    // classification or regression
    model->nr_feature = cols(input);
    model->nr_class   = model->options.solver > IM_CLASSORREG ? numberOfClass:1;
    model->nr_learner = numberOfClass; // one learner for two class or regression

    model->cost = (float*)  calloc(model->nr_learner,sizeof(float));
    model->bias = (float*)  calloc(model->nr_learner,sizeof(float));
    model->beta = (float**) calloc(model->nr_learner,sizeof(float*));

    float eps = model->options.epsilon;
    float lam = model->options.lambda;
    float eta = model->options.eta;
    int miter = model->options.max_iter;

    // if classification
    for (i=0; i < model->nr_learner; i++) {
        model->beta[i]      = (float*) calloc(cols(input), sizeof(float));
        // train linear classifier or regressor
        // bu değerler artık ouput->cols ile belli olacak. artık çoklu eğitim desteklencek
        sgd_solve(input, label, i, model->beta[i], &model->bias[i], model->options.solver, eps, eta, lam, miter);
    }

    return SUCCESS;
}

return_t glm_predict(matrix_t *input, matrix_t *label, struct glm_t *model) {

    int i,k, class;

    float (*tform)(float);
    int solv = model->options.solver;

    if(solv == LSRL1 || solv == LSRL2) { tform = lsr_transform; }
    if(solv == SVRL1 || solv == SVRL2) { tform = svr_transform; }
    if(solv == LREL1 || solv == LREL2) { tform = lre_transform; }
    if(solv == SVML1 || solv == SVML2) { tform = svm_transform; }

    for(class=0; class < model->nr_learner; class++) 
    {
        for(i=0; i < rows(input); i++)
        {
            // test each class
            float *data_s = data(float, input, i, 0);
            float lab = model->bias[class];

            for(k = 0; k < cols(input); k++ ) 
            {
                lab += model->beta[class][k]*data_s[k];
            }
            
            at(float, label, i, class) = tform(lab);
        }
    }

    // done
    return SUCCESS;
}

/// struct glm_t model import/export functions
struct glm_t* glm_read(const char *filename) {

    int i,j;
    // create the file with the given key value pair
    FILE *fp = imlab_fopen(filename, "rb", "glm");
    check_file(fp, NULL);

    struct glm_t *net = (struct glm_t*) calloc(1, sizeof(struct glm_t));

    fread(&net->nr_feature, sizeof(int), 1, fp);
    fread(&net->nr_class,   sizeof(int), 1, fp);
    fread(&net->nr_learner, sizeof(int), 1, fp);

    // now read the options
    fread(&net->options.solver,  sizeof(int),   1, fp);
    fread(&net->options.max_iter,sizeof(int),   1, fp);
    fread(&net->options.epsilon, sizeof(float), 1, fp);
    fread(&net->options.eta,     sizeof(float), 1, fp);
    fread(&net->options.lambda,  sizeof(float), 1, fp);

    // now read the learned coefficients
    net->beta = (float**) malloc(net->nr_learner*sizeof(float*));
    for(i=0; i < net->nr_learner; i++) {
        net->beta[i] = (float*) malloc(net->nr_feature*sizeof(float));
        for(j=0; j < net->nr_feature; j++) {
            fread(&net->beta[i][j], sizeof(float), 1, fp);
        }
    }
    net->bias = (float*) malloc(net->nr_learner*sizeof(float));
    net->cost = (float*) malloc(net->nr_learner*sizeof(float));
    for(i=0; i < net->nr_learner; i++) {
        fread(&net->bias[i], sizeof(float), 1, fp);
        fread(&net->cost[i], sizeof(float), 1, fp);
    }
    fclose(fp);
    //done
    return net;
}

return_t glm_write(struct glm_t *net, const char *filename) {

    int i,j;

    // open the file with the given key value pair
    FILE *fp = imlab_fopen(filename, "wb", "glm");
    check_file(fp);

    fwrite(&net->nr_feature, sizeof(int), 1, fp);
    fwrite(&net->nr_class,   sizeof(int), 1, fp);
    fwrite(&net->nr_learner, sizeof(int), 1, fp);

    // now print out the options
    fwrite(&net->options.solver,  sizeof(int),   1, fp);
    fwrite(&net->options.max_iter,sizeof(int),   1, fp);
    fwrite(&net->options.epsilon, sizeof(float), 1, fp);
    fwrite(&net->options.eta,     sizeof(float), 1, fp);
    fwrite(&net->options.lambda,  sizeof(float), 1, fp);
    // now print out the learned coefficients
    for(i=0; i < net->nr_learner; i++) {
        for(j=0; j < net->nr_feature; j++) {
            fwrite(&net->beta[i][j], sizeof(float), 1, fp);
        }
    }
    for(i=0; i < net->nr_learner; i++) {
        fwrite(&net->bias[i], sizeof(float), 1, fp);
        fwrite(&net->cost[i], sizeof(float), 1, fp);
    }
    fclose(fp);
    //done
    return SUCCESS;
}


/*
// HIGH LEVEL FUNCTIONS TO CALL FROM THE IMLAB APPLICATIONS
void glm_mpredict(FMAT *input, float *label, struct glm_t *model) {

    int i,j,k;

    float (*tform)(float);

    //if(model->options.solver == LSR) { tform = lsr_transform; }
    //if(model->options.solver == LRE) { tform = lre_transform; }
    //if(model->options.solver == SVM) { tform = svm_transform; }
    if(model->options.solver > IM_CLASSORREG) {
        for(j=0; j < input->rows; j++) {
            // test each class
            float max_prediction = -1000;
            for(i=0; i < model->nr_learner; i++) {

                float lab = model->bias[i];
                for(k = 0; k < input->cols; k++ ) {
                    lab += model->beta[i][k]*data(input)[input->cols*j+k];
                }
                //printf("Lab: %3.5f\n", lab);
                //lab = tform(lab);
                if( lab > max_prediction) { label[j] = i; max_prediction = lab; }

            }
        }
    } else {
        for(j=0; j < input->rows; j++) {
            // test each class
            float lab = model->bias[0];
            for(k = 0; k < input->cols; k++ ) {
                lab += model->beta[0][k]*data(input)[input->cols*j+k];
            }
            //printf("Lab: %3.5f\n", lab);
            //lab = tform(lab);
            label[j] = lab;
        }
    }
    // done
}
void glm_mtrain(FMAT *input, float *output, struct glm_t *model) {

    // Train One vs All classifiers
    int i,j;

    int nr_class = 1;
    // classification or regression
    if(model->options.solver > IM_CLASSORREG) {

        // count the elements in each class
        int *hist = (int*) calloc(IM_MAX_MULTI_CLASS, sizeof(int));
        for(i=0; i < input->rows; i++) {
            hist[ (int)output[i] ]++;
        }
        // count the number of class
        nr_class = 0;
        for(i=0; i < IM_MAX_MULTI_CLASS; i++) {
            if(hist[i] > 0) { nr_class++; }
        }
    }

    model->nr_feature = input->cols;
    model->nr_class   = nr_class;
    model->nr_learner = nr_class;
    // using ECOC will be increase the nr_classifier
    if(nr_class == 2) { model->nr_learner = 1; }

    model->cost = (float*)  calloc(model->nr_learner,sizeof(float));
    model->bias = (float*)  calloc(model->nr_learner,sizeof(float));
    model->beta = (float**) calloc(model->nr_learner,sizeof(float*));

    float eps = model->options.epsilon;
    float lam = model->options.lambda;
    float eta = model->options.eta;
    int miter = model->options.max_iter;

    // if classification
    if(model->options.solver > IM_CLASSORREG) {
        float *blabel = (float*) malloc(input->rows*sizeof(float));
        for (i=0; i < model->nr_learner; i++) {

            model->beta[i]      = (float*) calloc(input->cols, sizeof(float));
            int num_negative = 0;
            for (j=0; j < input->rows; j++) {
                // change the label for the ith class
                if(is_equal( (int)output[j],i,IM_EPS)) { blabel[j] = +1; }
                else if( num_negative < input->rows)   { blabel[j] = -1; num_negative++;}
                else { blabel[j] = 0; }
            }
            // train a classifier for the ith class
            // train linear support vector machine
            sgd_solve(data(input), blabel, model->beta[i], &model->bias[i], input->rows, input->cols, model->options.solver, eps, eta, lam, miter);
        }
        free(blabel);
    } else { // do regression on the output
        model->beta[0]      = (float*) calloc(input->cols, sizeof(float));
        // train a classifier for the ith class
        // train linear support vector machine
        sgd_solve(data(input), output, model->beta[0], &model->bias[0], input->rows, input->cols, model->options.solver, eps, eta, lam, miter);
    }
    //DONE
}
*/
