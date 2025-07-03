#include "../headers/BinaryTree.h"
#include <stdio.h>
#include <stdlib.h>

TreeNode* createNode(void* data)
{
  TreeNode* node = malloc(sizeof(TreeNode));
  if(node == NULL) {
    perror("Failed to allocate TreeNode");
    return NULL;
  }
  node->data = data;
  node->left = node->right = NULL;
  return node;
}

TreeNode* insertNode (TreeNode* root, void* data,CompareFunc cmp) 
{
  if(root == NULL) {
    return createNode(data);
  }
  int result = cmp(data,root->data);
  if(result <0) root->left = insertNode(root->left,data,cmp);
  else if(result > 0) root->right = insertNode(root->right,data,cmp);
  return root;
}

void insertBST(BinaryTree *tree,void *data) {
  if(tree==NULL) return;
  tree->root = insertNode(tree->root,data,tree->cmp);
}

void inorderTraversal(TreeNode*root,PrintFunc print) {
  if(root == NULL) return;

  inorderTraversal(root->left,print);
  print(root->data);
  inorderTraversal(root->right,print);
}

void destroyTree(TreeNode *root,FreeFunc destroy) {
  if(root == NULL) return;

  destroyTree(root->left,destroy);
  destroyTree(root->right,destroy);
  if(destroy) destroy(root->data);
  free(root);
}

BinaryTree initTree(CompareFunc cmp,PrintFunc print,FreeFunc destroy) {
  BinaryTree tree;
  tree.root = NULL;
  tree.cmp = cmp;
  tree.print = print;
  tree.destroy = destroy;
  return tree;
}