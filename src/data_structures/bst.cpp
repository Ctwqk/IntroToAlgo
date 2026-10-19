#include <iostream>
#include <stack>
#include <vector>
using namespace std;
/*below is bst*/
class TreeNode{
public:
    int val;
    TreeNode *p,*left,*right;
    TreeNode() {val=0;left=nullptr;right=nullptr;p=nullptr;}
    TreeNode (int va) {val=va;left=nullptr;right=nullptr;p=nullptr;}
    TreeNode (int va, TreeNode* par) {val=va; p=par;right=nullptr;left=nullptr;}
    TreeNode (int va, TreeNode* lef,TreeNode *righ){val=va;left=lef;right=righ;}
    TreeNode (int va,TreeNode* par,TreeNode* lef,TreeNode* righ){val=va;p=par;left=lef;right=righ;}
    void insert(int val);
    void insert(TreeNode* node);
    void inorder_traverse();
    TreeNode* getMin();
    TreeNode* getMax();
    TreeNode* getPredecessor(TreeNode* root);
    TreeNode* getSuccessor(TreeNode* root);
    TreeNode* search(int val);
    int getDepth();
    void delNode(TreeNode* root);
    void delNode(int val);
    void delNode();
    void alternative(TreeNode* alt);
    void change(int va1,int va2);
    void threading();
};
void TreeNode::insert(int val){   // (The elements with same value in the left subtree)
    this->insert(new TreeNode(val));
}
void TreeNode::insert(TreeNode* node){
    TreeNode* q=this;
    while(q){
        if(node->val>q->val){
            if(q->right) q=q->right;
            else {
                q->right=node;
                node->p=q;
                break;
            }
        }
        else{
            if(q->left) q=q->left;
            else {
                q->left=node;
                node->p=q;
                break;
            }
        }
    }
}
void TreeNode::inorder_traverse(){
    TreeNode *root=this;
    stack<TreeNode*> tree;
    while(root||tree.size()){
        while(root){
            tree.push(root);
            root=root->left;
        }
        root=tree.top();
        tree.pop();
        cout<<root->val<<endl;
        root=root->right;
    }
    cout<<"done"<<endl;
}
TreeNode* TreeNode::getMin(){
    TreeNode* root=this;
    while(!!root->left) root=root->left;
    return root;
}
TreeNode* TreeNode::getMax(){
    TreeNode* root=this;
    while(!!root->right) root=root->right;
    return root;
}
TreeNode* TreeNode::getPredecessor(TreeNode *root){
    if(!root) return nullptr;
    return root->left->getMax();
}
TreeNode* TreeNode::getSuccessor(TreeNode *root){
    if(!root) return nullptr;
    return root->right->getMin();
}
TreeNode* TreeNode::search(int va){
    TreeNode* root=this;
    while(root){
        if(root->val==va) return root;
        if(va>root->val) root=root->right;
        else root=root->left;
    }
    cout<<"no element with such value in tree"<<endl;
    return nullptr;
}
int TreeNode::getDepth(){
    TreeNode *root=this;
    int i=1;
    while(root->p) {
        root=root->p;
        i++;
    }
}
void TreeNode::delNode(TreeNode* root){
    root->delNode();
}
void TreeNode::delNode(int val){
    this->search(val)->delNode();
}
void TreeNode::delNode(){
    TreeNode* root=this;
    if(!root->left) {
        root->alternative(root->right);
        return;
    }
    if(!root->right) {
        root->alternative(root->left);
        return;
    }
    TreeNode* q=TreeNode::getPredecessor(root);
    TreeNode* s=TreeNode::getSuccessor(root);
    TreeNode* tar=nullptr;
    if(!q&&!s) root->alternative(nullptr);
    else if(q&&s) q->getDepth()>s->getDepth()?tar=q:tar=s;
    else q?tar=q:tar=s;
    if(tar==q) tar->alternative(tar->left);
    else tar->alternative(tar->right);
    root->alternative(tar);
    tar->left->alternative(root->left);
    tar->right->alternative(root->right);
}
void TreeNode::alternative(TreeNode* alt){
    if(this->p) this->p->left==this?this->p->left=alt:this->p->right=alt;
    if(alt) alt->p=this->p;
}
void TreeNode::change(int va1,int va2){
    TreeNode* target=this->search(va1);
    if(!target) {
        cout<<"change failed"<<endl;
        return;
    }
    target->val=va2;
    this->delNode(target);
    this->insert(target);
}
/*below is bbst*/
class BbstNode:public TreeNode{
public:
    BbstNode *p,*left,*right;
    BbstNode(){}
    void leftRotate();
    void rightRotate();
    void leftRightRotate();
    void rightLeftRotate();
};
void BbstNode::leftRotate(){
    BbstNode *root=this,*righ=root->right;
    if(!righ) {
        cout<<"invalid operation: right subtree=null"<<endl;
        return;
    }
    root->alternative(righ);
    root->right=righ->left;
    righ->left=root;
    root->p=righ;
    if(!!righ->left) righ->left->p=righ;
}
void BbstNode::rightRotate(){
    BbstNode* root=this,*lef=root->left;
    if(!lef) {
        cout<<"invalid operation: right subtree=null"<<endl;
        return;
    }
    root->alternative(lef);
    root->left=lef->right;
    lef->right=root;
    root->p=lef;
    if(!!lef->right) lef->right->p=lef;
}
void BbstNode::leftRightRotate(){  //to be check
    BbstNode *root=this;
    if(!this->left) {
        cout<<"invalid operation: right subtree=null"<<endl;
        return;
    }
    this->left->leftRotate();
    this->rightRotate();
}
void BbstNode::rightLeftRotate(){
    BbstNode *root=this;
    if(!this->right) {
        cout<<"invalid operation: right subtree=null"<<endl;
        return;
    }
    this->right->rightRotate();
    this->leftRotate();
}
/*below is rb tree*/
class RbNode : public BbstNode {
private:
    static int counting;
public:
    int color; // 1:black, 0:red
    RbNode *p, *left, *right;
    static RbNode Nil;
    static void sta_init(){
        RbNode::Nil.color = 1;
        counting=0;
    }
    RbNode() {
        left = &Nil;
        right = &Nil;
        p = &Nil;
        color = 0;
        if(!counting++) {
            Nil.left=this;
            this->color=1;
        }
    }
    
    bool operator! () {
        return (this == &Nil);
    }
    void insert(RbNode *tar);
    void insert_fixup();
    void delNode();
    void delete_fixup(RbNode *lab);
};
RbNode RbNode::Nil;
int RbNode::counting;
void RbNode::insert(RbNode * tar){
    if(!this) return;
    if(tar==nullptr){
        cout<<"wtf"<<endl;
        return;
    }
    RbNode *root=this;
    while(true){
        if(root->val>=tar->val){
            if(!(root->left)) {
                root->left=tar;
                tar->p=root;
            }
            else root=root->left;
        }
        else{
            if(!(root->right)){
                root->right=tar;
                tar->p=root;
            }
            else root=root->right;
        }
    }
    tar->insert_fixup();
}
void RbNode::insert_fixup(){
    RbNode *root=this;
    if(!this) return;
    while(!!root->p->p&&!root->p->color){
        RbNode *u;
        if(root->p==root->p->p->left){
            u=root->p->p->right;
            if(u->color==0){
                u->p->color=0;
                u->color=1;
                root->p->color=1;
                root=root->p;
            }
            else{
                if(root=root->p->right){
                    root=root->p;
                    root->leftRotate();
                }
                root->p->color=1;
                root->p->p->color=0;
                root->p->p->rightRotate();
            }
        }
        if(root->p==root->p->p->right){
            u=root->p->p->left;
            if(u->color=0){
                u->color=1;
                u->p->color=0;
                root->p->color=1;
                root=root->p;
            }
            else{
                if(root->p->left=root){
                    root=root->p;
                    root->rightRotate();
                }
                root->p->color=1;
                root->p->p->color=0;
                root->p->p->leftRotate();
            }
        }
    }
    RbNode::Nil.left->color=1;
}
void RbNode::delNode(){
    RbNode *root=this,*lab=nullptr;
    if(!this) return;
    if(!root->left) {
        root->alternative(root->right);
        lab=root->right;
        return;
    }
    if(!root->right){
        root->alternative(root->left);
        lab=root->left;
        return;
    }
    RbNode* s=(RbNode*)getSuccessor(root);
    RbNode* q=(RbNode*)getPredecessor(root);
    RbNode* tar=nullptr;
    s->color==0?tar=s:tar=q;
    if(tar==q){
        lab=tar->left;
    }
    else {
        lab=tar->right;
    }
    tar->alternative(lab);
    root->alternative(tar);
    tar->left->alternative(root->left);
    tar->right->alternative(root->right);
    int c=tar->color;
    tar->color=root->color;
    if(c) delete_fixup(lab);
}
void RbNode::delete_fixup(RbNode *lab){
    while(lab->color&&lab->p!=&Nil){
        RbNode* u;
        if(lab==lab->p->left){
            u=lab->p->right;
            if(u->color==0){
                u->color=1;
                u->p->color=0;
                u->p->leftRotate();
                u=lab->p->right;
            }
            if(u->left->color==1&&u->left->color==1){
                u->color=0;
                lab=u->p;
            }
            else {
                if(u->right->color==1){
                    u->left->color=1;
                    u->color=0;
                    u->rightRotate();
                    u=lab->p->right;
                }
                u->color=lab->p->color;
                lab->p->color=1;
                u->right->color=1;
                lab->p->leftRotate();
                lab=Nil.left;
            }
        }
        if(lab=lab->p->right){
            u=lab->p->left;
            if(u->color=0){
                u->color=1;
                u->p->color=0;
                u->p->rightRotate();
                u=lab->p->left;
            }
            if(u->left->color==1&&u->right->color==1){
                u->color=0;
                lab=u->p;
            }
            else{
                if(u->left->color=1){
                    u->right->color=1;
                    u->color=0;
                    u->leftRotate();
                    u=lab->p->left;
                }
                u->color=u->p->color;
                lab->p->color=1;
                u->left->color=1;
                u->p->rightRotate();
                lab=Nil.left;
            }
        }
    }
    lab->color=1;
}
/*below is AVL*/
/*below is test*/
void test_bst(){
    vector<int> values{10,33,44,1323,42,99,0,1,4,29,4,5};
    TreeNode* root=new TreeNode(values[0]);
    TreeNode *tmp=root;
    for(int i=1;i<values.size();i++) {
        tmp->insert(values[i]);
    }
    root->inorder_traverse();
    root->change(1323,11);
    root->inorder_traverse();
}
void test_RB(){
    RbNode::sta_init();
    vector<int> values{10,33,44,1323,42,99,0,1,4,29,4,5};
    TreeNode* root=new TreeNode(values[0]);
    TreeNode *tmp=root;
    for(int i=1;i<values.size();i++) {
        tmp->insert(values[i]);
    }
    root->inorder_traverse();
    root->inorder_traverse();
}
int main(){
    test_RB();
}
