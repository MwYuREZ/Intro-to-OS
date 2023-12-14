/*
Student No.: 110550065
Student Name: Mao Wei Yu
Email: zxcv9907800@gmail.com
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not
supposed to be posted to a public server, such as a
public GitHub repository or a public web page.
*/
#include <iostream>
#include <sys/time.h>
#include <vector>
#include <algorithm>

using namespace std;
///////////for LRU
struct rnode{
    int addr;
    rnode *prev;
    rnode *next;
};

///////////////////

///////////for LFU addr//////////////
struct node{
    int addr;
    int fre;
    int seq;
    struct node *left;
    struct node *right;
};

struct node* newNode(int _addr,int _fre,int _seq){//create new BST node
    struct node* tmp=new struct node;
    tmp->addr=_addr;
    tmp->fre=_fre;
    tmp->seq=_seq;
    tmp->left=nullptr;
    tmp->right=nullptr;
    return tmp;
}

struct node* insert(struct node *node , int _addr,int _fre,int _seq){
    if(node==NULL){
        return newNode(_addr,_fre,_seq);
    }
    if(_addr < node->addr){
        node->left=insert(node->left,_addr,_fre,_seq);
    }else if(_addr > node->addr){
        node->right=insert(node->right,_addr,_fre,_seq);
    }

    return node;
}

struct node* deletenode(struct node* root, int k)
{
    // Base case
    if (root == nullptr)
        return root;
 
    // Recursive calls for ancestors of
    // node to be deleted
    if (root->addr > k) {
        root->left = deletenode(root->left, k);
        return root;
    }
    else if (root->addr < k) {
        root->right = deletenode(root->right, k);
        return root;
    }
 
    // We reach here when root is the node
    // to be deleted.
 
    // If one of the children is empty
    if (root->left == nullptr) {
        node* tmp = root->right;
        delete root;
        return tmp;
    }
    else if (root->right == nullptr) {
        node* tmp = root->left;
        delete root;
        return tmp;
    }
 
    // If both children exist
    else {
 
        node* succParent = root;
 
        // Find successor
        node* succ = root->right;
        while (succ->left != nullptr) {
            succParent = succ;
            succ = succ->left;
        }
 
        if (succParent != root)
            succParent->left = succ->right;
        else
            succParent->right = succ->right;
 
        // Copy Successor Data to root
        root->addr = succ->addr;
        root->seq  = succ->seq;
        root->fre  = succ->fre;
 
        // Delete Successor and return root
        delete succ;
        return root;
    }
}


struct node* search(struct node* node, int target){
    if(node==nullptr){
        return nullptr;
    }
    if(node->addr==target){
        return node;
    }
    if(node->addr < target){
        return search(node->right, target);
    }
    return search(node->left , target);
}

////////////////for frequently,seq_num////////////////
struct Node {
    int fre;
    int seq;
    int addr; // 新增的 addr
    Node* left;
    Node* right;
    
    Node(int k1, int k2, int a) : fre(k1), seq(k2), addr(a), left(nullptr), right(nullptr) {}
};

// 插入節點到二元搜尋樹
Node* insertNode(Node* root, int _fre, int _seq, int addr) {
    if (root == nullptr) {
        return new Node(_fre, _seq, addr);
    }

    if (_fre < root->fre || (_fre == root->fre && _seq < root->seq)) {
        root->left = insertNode(root->left, _fre, _seq, addr);
    } else {
        root->right = insertNode(root->right, _fre, _seq, addr);
    }

    return root;
}

// 找出最小的節點及其 addr
Node* findMin(Node* root, int& minAddr) {
    while (root->left != nullptr) {
        root = root->left;
    }
    minAddr = root->addr;
    return root;
}

// 刪除節點
Node* deleteNode(Node* root, int _fre, int _seq) {

    if (root == nullptr) {
        return root;
    }

    if (_fre < root->fre || (_fre == root->fre && _seq < root->seq)) {
        root->left = deleteNode(root->left, _fre, _seq);
        return root;
    } else if (_fre > root->fre || (_fre == root->fre && _seq > root->seq)) {
        root->right = deleteNode(root->right, _fre, _seq);
        return root;
    } 

    // We reach here when root is the node
    // to be deleted.
    
 
    // If one of the children is empty
    if (root->left == nullptr) {
        Node* tmp = root->right;
        delete root;
        return tmp;
    }
    else if (root->right == nullptr) {
        Node* tmp = root->left;
        delete root;
        return tmp;
    }else {
 
        Node* succParent = root;
 
        // Find successor
        Node* succ = root->right;
        while (succ->left != nullptr) {
            succParent = succ;
            succ = succ->left;
        }
 
        if (succParent != root)
            succParent->left = succ->right;
        else
            succParent->right = succ->right;
 
        // Copy Successor Data to root
        root->addr = succ->addr;
        root->fre  =succ->fre;
        root->seq  =succ->seq;
 
        // Delete Successor and return root
        delete succ;
        return root;
    }
}
void inorderTraversal(Node* root) {
    if (root != nullptr) {
        inorderTraversal(root->left);
        cout << "Fre: " << root->fre << ", Seq: " << root->seq << ", Addr: " << root->addr << endl;
        inorderTraversal(root->right);
    }
}
void inordertraversal(node* root) {
    if (root != nullptr) {
        inordertraversal(root->left);
        cout << "Addr: " << root->addr << ", fre: " << root->fre << ", seq: " << root->seq << endl;
        inordertraversal(root->right);
    }
}
///////////////////////////

int main(int argc,char **argv){
    FILE *inputf;
    int addr;
    //int first=1;

    int frame[4]={64,128,256,512};

    //////////////LFU/////////////////////////////////
    printf("LFU policy:\n");
    printf("Frame\tHit\t\tMiss\t\tPage fault ratio\n");
    struct timeval start1,end1;
    gettimeofday(&start1,0);
    for(int i=0;i<=3;i++){
        int first=1;
        int hit_count=0;
        int miss_count=0;
        int frames=frame[i];
        int use=0;
        int seq=0;
        inputf=fopen(argv[1],"r");

        struct node* root=nullptr;
        struct Node* Root=nullptr;



        while(fscanf(inputf,"%d",&addr)==1){
            seq++;
            if(first){
                first--;
                
                root=insert(root,addr,1,seq);//add

                Root=insertNode(Root,1,seq,addr);//feq seq

                miss_count++;
                use++;
                continue;
            }
            //printf("%d:",seq);
            //printf("\n");

            node *find=search(root,addr);

            if(find!=nullptr){//find it 
                //printf("hit\n");
                hit_count++;
                //deleteNode(Root,find->fre,find->seq);//update

                if(Root->addr==addr){
                    Root=deleteNode(Root,find->fre,find->seq);
                }else{
                    deleteNode(Root,find->fre,find->seq);
                }
                //inorderTraversal(Root);
                //printf("\n");                

                find->fre=find->fre+1;
                find->seq=seq;

                insertNode(Root,find->fre,find->seq,addr);

            }else{//no exist
                //printf("miss\n");
                miss_count++;
                if(use<frames){//have free slot
                    use++;
                    insert(root,addr,1,seq);
                    insertNode(Root,1,seq,addr);

                }else{
                    //find victim
                    int minaddr;
                    Node *victim=findMin(Root,minaddr);
                    int victim_addr=victim->addr;

                    //delete victim
                    if(victim_addr==root->addr){
                        root=deletenode(root,victim_addr);
                    }else{
                        deletenode(root,victim_addr);
                    }
                    
                    if(victim_addr==Root->addr){
                        Root=deleteNode(Root,victim->fre,victim->seq);
                    }else{
                        deleteNode(Root,victim->fre,victim->seq);
                    }
                    

                    //add new
                    insert(root,addr,1,seq);
                    insertNode(Root,1,seq,addr);
                }
            }
            // inordertraversal(root);
            // printf("\n");
            // inorderTraversal(Root);
            // printf("\n");
        }
        //float f_ratio=float(miss_count)/float(hit_count+miss_count);
        printf("%d\t%d\t\t%d\t\t%.10f\n",frames,hit_count,miss_count,miss_count*1./(miss_count+hit_count));
        fclose(inputf);
    }
    gettimeofday(&end1,0);
    double sec=(end1.tv_sec-start1.tv_sec)+(end1.tv_usec-start1.tv_usec)/1e6;
    printf("Total elapsed time %.4f sec\n\n",sec);


    //////////////////LRU///////////////////////////////////////
    printf("LRU policy:\n");
    printf("Frame\tHit\t\tMiss\t\tPage fault ratio\n");
    struct timeval start2,end2;
    gettimeofday(&start2,0);

    for(int i=0;i<=3;i++){
        int first=1;
        int frames=frame[i];
        int hit_count=0;
        int miss_count=0;
        int use=0;

        inputf=fopen(argv[1],"r");

        vector<pair<int,rnode*>> hash[600];
        struct rnode *head=nullptr;
        struct rnode *tail=nullptr;
        struct rnode *cur;

        while(fscanf(inputf,"%d",&addr)==1){
            if(first){
                first--;
                struct rnode *tmp=new rnode;
                tmp->addr=addr;
                tmp->prev=nullptr;
                tmp->next=nullptr;
                head=tmp;
                tail=head;
                hash[(addr%frames)].push_back({addr,tmp});

                miss_count++;
                use++;
                continue;
            }

            
            cur=nullptr;
            for(auto &f:hash[(addr%frames)]){
                if(f.first==addr){
                    cur=f.second;
                }
            }

            if(cur!=nullptr){//find it
                hit_count++;
                if(head!=cur){
                    cur->prev->next=cur->next;//prepare to remove
                }else{//no need to update
                    continue;
                }
                
                if(tail!=cur){//for new tail
                    cur->next->prev=cur->prev;
                }else{
                    tail=cur->prev;//new tail
                    tail->next=nullptr;
                }

                //put to head
                cur->prev=nullptr;
                cur->next=head;
                head->prev=cur;
                head=cur;
            }else{
                miss_count++;
                if(use<frames){
                    struct rnode *tmp=new rnode;
                    tmp->addr=addr;
                    tmp->prev=nullptr;
                    tmp->next=head;
                    head->prev=tmp;
                    head=tmp;
                    hash[(addr%frames)].push_back({addr,tmp});
                    use++;
                }else{
                    //victim
                    auto victim_val=make_pair(tail->addr,tail);
                    auto &tg_hash=hash[tail->addr%frames];
                    tg_hash.erase(remove(tg_hash.begin(),tg_hash.end(),victim_val),tg_hash.end());
                    tail=tail->prev;
                    tail->next=nullptr;

                    //new to head
                    struct rnode *tmp=new rnode;
                    tmp->addr=addr;
                    tmp->prev=nullptr;
                    tmp->next=head;
                    head->prev=tmp;
                    head=tmp;
                    hash[(addr%frames)].push_back({addr,tmp});
                }

            }
        }
        printf("%d\t%d\t\t%d\t\t%.10f\n",frames,hit_count,miss_count,miss_count*1./(miss_count+hit_count));
        fclose(inputf);
    }
    gettimeofday(&end2,0);
    sec=(end2.tv_sec-start2.tv_sec)+(end2.tv_usec-start2.tv_usec)/1e6;
    printf("Total elapsed time %.4f sec\n\n",sec);
}