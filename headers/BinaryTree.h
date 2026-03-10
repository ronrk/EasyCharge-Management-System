#ifndef BINARYTREE_H
#define BINARYTREE_H

// Function Pointers (for making this binary tree for stations and for car):
typedef int (*CompareFunc)(const void *, const void *);
typedef void (*PrintFunc)(const void *);
typedef void (*FreeFunc)(void *);

// STRUCT & Enums declarations
typedef struct TreeNode
{
  struct TreeNode *left;
  struct TreeNode *right;
  void *data;
} TreeNode;

typedef struct BinaryTree
{
  TreeNode *root;

  CompareFunc cmp;
  PrintFunc print;
  FreeFunc destroy;
} BinaryTree;

// FUNCTIONS
BinaryTree initTree(CompareFunc cmp, PrintFunc print, FreeFunc destroy);
int insertBST(BinaryTree *tree, void *data);
void *searchBST(BinaryTree *tree, const void *key);
void destroyTree(TreeNode *root, FreeFunc destroy);
void inorderBST(BinaryTree *tree, void (*printFunc)(const void *));
int deleteBST(BinaryTree* tree,const void* key);

#endif
