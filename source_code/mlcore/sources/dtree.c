/*

//Decision Tree Model
typedef struct DTM
{
    int i;
};

// CART Algorithm
typedef struct TreeNode
{
    TreeNode *left, *right; // right left sub-trees
    int NumLeft, NumRight, Feature;
    VECF expected;          // expected output at the current node
    double threshold, gain;
}TreeNode;

float *class_distribution(float *label, int nr_sample, int *nr_class) {

    int i,j = 0;
    // count the elements in each class
    int *hist = (int*) calloc(*nr_class, sizeof(int));
    for(i=0; i < nr_sample; i++) { hist[ ImRound(label[i]) ]++; }
    // count the numbers of class
    int nrclass = 0;
    for(i=0; i < *nr_class; i++) { if(hist[i] > 0) { nrclass++; } }

    // these are the output variables for the problem
    float *distribution = (float*) malloc(nrclass*sizeof(float));

    // compute the distribution and class names
    for(i=0; i < nrclass; i++) {
        distribution[i] = (float)hist[i] / nr_sample;
    }
    free(hist);
    // return the output variables
    *nr_class = nrclass;
    return distribution;
}

float gini_impurity(float *label, int nr_sample) {

    int nr_class = IM_MAX_MULTI_CLASS; // assume that the maximum number of class is limited
    // get class probabilities and number of class
    float *prob = class_distribution(label, nr_sample, &nr_class);
    float gini = 1.0f;
    for(i=0; i < nr_class; i++) {
        gini -= prob[i]*prob[i];
    }
    return gini;
}

float gini_gain(float *feature, float *label, int nr_sample) {

    int i;
}

// Split function
int get_best_split(float *feature, float *output, int *idx, int NumSample, int NumFeatures, int *NumPos, int *NumNeg)
{
    int i,s;
    int S;
    float minMSE = 100000;

    for(s=0; s < NumFeatures; s++) {

        float xixt = 0, xi2t = 0;
        float xixf = 0, xi2f = 0;
        int St = 0, Sf = 0;

        for(i=0; i < NumSample; i++) {
            // compute class variances based on feature s
            if( feature[s + idx[i]*NumFeatures] > 0.5 ) {
                    xixt += output[idx[i]];
                    xi2t += output[idx[i]]*output[idx[i]];
                    St ++;
            }
            else {
                xixf += output[idx[i]];
                xi2f += output[idx[i]]*output[idx[i]];
                Sf ++;
            }
        }
        float VarT = St > 0 ? (xi2t-xixt*xixt/St) / St : 0;
        float VarF = Sf > 0 ? (xi2f-xixf*xixf/Sf) / Sf : 0;
        //printf("Var[%d]: %3.5f\n", s,VarT+VarF);
        if(St==0 || Sf==0) { continue; }
        if(minMSE > (VarT+VarF)) { minMSE = VarT+VarF; S = s; *NumPos = St; *NumNeg = Sf; }
    }
    return S;
}
// Regression function
void split_two(float *feature, float *output, int *idx, int node_idx, int depth, int max_depth, int NumChild, int NumFeatures, int **node, float **response) {

    int i,j, NumPos, NumNeg;
    int posi = 0, negi = 0;

    float LeafResponse = 0, SleafResponse = 0, LeafVariance;
    for(i=0; i < NumChild; i++) {
            LeafResponse +=  output[idx[i]];
            SleafResponse += output[idx[i]]*output[idx[i]];
    }
    (*node)[node_idx] = -1;
    (*response)[node_idx] = LeafResponse/(1000+NumChild);
    LeafVariance = SleafResponse - LeafResponse*LeafResponse/NumChild;
    // we are in the terminal node
    if  (depth == max_depth-1 || LeafVariance < 0.00001) { return; }

    (*node)[node_idx] = get_best_split(feature, output, idx, NumChild, NumFeatures, &NumPos, &NumNeg);

    int *pidx = (int*) calloc(NumPos,sizeof(int));
    int *nidx = (int*) calloc(NumNeg,sizeof(int));
    // split features and labels
    for(i=0; i < NumChild; i++) {
        if (feature[(*node)[node_idx] + NumFeatures*idx[i]] > 0.5) { pidx[posi++] = idx[i]; }
        else { nidx[negi++] = idx[i]; }
    }
    // recursively call split_two function
    split_two(feature, output, nidx, 2*node_idx+0, depth+1, max_depth, NumNeg, NumFeatures, node, response);
    split_two(feature, output, pidx, 2*node_idx+1, depth+1, max_depth, NumPos, NumFeatures, node, response);

}

TreeNode *CART_regression(MATF *feature, double *label, int NumSample)
{
    // feature->height : Number of Samples
    // feature->width  : Number of Features
    // NumSample       : size of the label pointer == feature->height
    if(feature->height != NumSample) { printf("Features and labels have different size!\n"); return; }
    int NumFeature = feature->width;
    // Index for all elements in the input
    int *AllMatrix = (int*) malloc(NumSample*sizeof(int));
    for(n=0; n < NumSample; n++) { AllMatrix[n] = n; }

    TreeNode *CART = (TreeNode*) malloc(sizeof(TreeNode));

    split_two(&CART, feature, label, AllMatrix, node_idx, depth, max_depth, NumSample, NumFeature);


}
*/