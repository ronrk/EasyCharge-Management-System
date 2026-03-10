#include "BinaryTree.h"
#include "ErrorHandler.h"

#include <stdio.h>
#include <stdlib.h>

// STATIC

// create dynamic node
static TreeNode* createNode(void* data)
{
  // Allocate memory newNode
  TreeNode* node = malloc(sizeof(TreeNode));
  // Check Allocation
  if(node == NULL) {
    displayError(ERR_MEMORY,"Failed to allocate TreeNode");
    return NULL;
  }
  // Init treeNode
  node->data = data;
  node->left = node->right = NULL;
  return node;
}

// insert node to tree
static TreeNode* insertNode (TreeNode* root, void* data,CompareFunc cmp) 
{
  if(root == NULL) {
    // if in a leaf
    TreeNode *node = createNode(data);

    return node;
  }

  // compare newNode data with current node 
  int result = cmp(data,root->data);

  // insert into left
  if(result < 0) {

    root->left = insertNode(root->left,data,cmp);}

  else if(result > 0) {
// insert into right
    root->right = insertNode(root->right,data,cmp);
  }
  return root;
}

// inorder traversall with custom print function
static void inorderNode(TreeNode *node, void (*printFunc)(const void *)) {
    if (!node) return;
    inorderNode(node->left, printFunc);
    printFunc(node->data);
    inorderNode(node->right, printFunc);
}

// delete node
static TreeNode* deleteNode(TreeNode* root, const void* key,CompareFunc cmp,FreeFunc destroy){
  if(!root) return NULL;
  int res = cmp(key,root->data);

  if(res < 0) {
    root->left = deleteNode(root->left,key,cmp,destroy);
  } else if (res > 0) {
    root->right = deleteNode(root->right,key,cmp,destroy);
  } else {
    // FOUND NODE TO REMOVE
    // node with 1 children or no children
    if(!root->left){
      TreeNode* tmp = root->right;
      if(destroy) destroy(root->data);
      free(root);
      return tmp;
    }

    // node with 2 children
    TreeNode* succ = root->right;
    while (succ->left)
    {
      succ = succ->left;
    }

    // copy data into current
    void* tmpData = root->data;
    root->data = succ->data;
    succ->data = tmpData;

    root->right = deleteNode(root->right,succ->data,cmp,destroy);
    

  }

  return root;
}

// FUNCTIONS 

// initialize new tree
BinaryTree initTree(CompareFunc cmp, PrintFunc print, FreeFunc destroy) {
  BinaryTree tree;
  tree.root = NULL;
  tree.cmp = cmp;
  tree.print = print;
  tree.destroy = destroy;
  return tree;
}

// public insert node to a tree
int insertBST(BinaryTree *tree,void *data) {
  if(tree==NULL) {
    displayError(ERR_LOADING_DATA,"[insertBST]:NULL tree pointer");
    return 0;
  }

  // insertion
  tree->root = insertNode(tree->root,data,tree->cmp);
  return 1;
}

// inorder search in tree
void * searchBST(BinaryTree *tree, const void* key) {
  if(!tree||!tree->root||!key) return NULL;
  
  TreeNode *current= tree->root;

  while (current)
  {
    int cmp = tree->cmp(key,current->data);
    if(cmp == 0) {
      return current->data;
    }
    current = (cmp<0) ? current->left : current->right;
  }
  return NULL;
  
}

// public inorder tree traversall with custom print function
void inorderBST(BinaryTree *tree, void (*printFunc)(const void *)) {
    if (!tree || !printFunc) return;
    inorderNode(tree->root, printFunc);
}

// public destroy the tree
void destroyTree(TreeNode *root,FreeFunc destroy) {
  if(root == NULL) return;

  destroyTree(root->left,destroy); // left

  destroyTree(root->right,destroy); // right

  if(destroy) 
    destroy(root->data);

  free(root);
}

// delet item from tree
int deleteBST(BinaryTree* tree,const void* key){
  if(!tree || !tree->root) return 0;

  tree->root = deleteNode(tree->root,key,tree->cmp,tree->destroy);
  return 1;
}
