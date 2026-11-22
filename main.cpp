// this code adds two numbers together!
#pragma GCC optimize("Ofast,unroll-loops")
#include <bits/stdc++.h>
#pragma GCC target("avx2")
#define int long long
using namespace std;
struct treap {
     int total;
     int subsum;
     int prioriy;
     int element;
     treap * l;
     treap * r;
     void fix() {
          subsum = 1;
          if(l!=NULL) subsum += l->subsum;
          if(r!=NULL) subsum += r->subsum;
          total = element;
          if(l!=NULL) total += l->total;
          if(r!=NULL) total += r->total;
     }
};
treap * merge(treap * a, treap * b) {
     if(a == NULL) return b;
     if(b == NULL) return a;
     if(a->prioriy > b->prioriy) {
          if(a->r == NULL) a->r = b;
          else a->r = merge(a->r, b);
          a-> fix();
          return a;
     }else {
          if(b->l == NULL) b->l = a;
          else b->l = merge(a, b->l);
          b->fix();
          return b;
     }
}
pair<treap *, treap *> split(treap * a, int subsum) {
     if(subsum == 0) return {NULL, a};
     if(a == NULL) return {NULL, NULL};
     int leftsize = 0;
     if(a -> l != NULL) leftsize = a->l->subsum;
     if(subsum <= leftsize) {
          pair<treap *, treap *> b = split(a->l, subsum);
          a->l = b.second;
          a->fix();
          return {b.first, a};
     }else {
          pair<treap *, treap *> b = split(a->r, subsum - leftsize - 1);
          a->r = b.first;
          a->fix();
          return {a, b.second};
     }
}
const int MAXN = 200005;
treap arr[MAXN];
mt19937 Rand(random_device{}());
treap * tre = NULL;
int query(int pos) {
     pair<treap *, treap *> a = split(tre, pos);
     pair<treap *, treap *> b = split(a.second, 1);
     int ans = b.first->element;
     tre = merge(a.first, merge(b.first, b.second));
     return ans;
}
vector<int> neighbors[MAXN];
vector<pair<int, bool>> children[MAXN]; // true is heavy, false is light
bool seenroot[MAXN];
int subsum[MAXN];
int depth[MAXN];
pair<int, bool> parent[MAXN];
void root(int node, int d) {
     depth[node] = d;
     seenroot[node] = true;
     subsum[node] = 1;
     for(int i : neighbors[node]) {
          if(!seenroot[i]) {
               children[node].push_back({i, 67});
               root(i, d+1);
               parent[i] = {node, 67};
               subsum[node] += subsum[i];
          }
     }
}
int tree[MAXN * 2];
void upd(int node, int val) {
     tree[node += MAXN] = val;
     for(node /= 2; node > 0; node /= 2) tree[node] = max(tree[node * 2], tree[node * 2 + 1]);
}
int query(int left, int right) {
     int maxval = 0;
     left += MAXN;
     right += MAXN;
     while(left < right) {
          if(left & 1) maxval += tree[left ++];
          if(right & 1) maxval +=  tree[--right];
          left /=2;
          right /= 2;
     }
     return maxval;
}
vector<pair<int, int>> hldranges;
vector<vector<int>> hldpaths;
pair<int, int> loc[MAXN];
int numchildren[MAXN];
void change(int node, int val) {
     int path = loc[node].first;
     int locationinpath = loc[node].second;
     upd(hldranges[path].first + locationinpath, val);
}
int jump[MAXN][20];
int n, q;
void calc() {
     for(int i = 1; i <= n; i++) {
          jump[i][0] = parent[i].first;;
     }
     for(int i = 1; i < 20; i++) {
          for(int j = 1; j <= n; j++) {
               jump[j][i] = jump[jump[j][i-1]][i-1];
          }
     }
}
int moveup(int node, int dist) {
     for(int i = 19; i >= 0; i--) {
          if(dist >= (1<<i)) {
               node = jump[node][i];
               dist -= (1<<i);
          }
     }
     return node;
}
int lca(int a, int b) {
     if(depth[a] < depth[b]) {
          b = moveup(b, depth[b] - depth[a]);
     }else if(depth[a] > depth[b]) {
          a = moveup(a, depth[a] - depth[b]);
     }
     if(a == b) return a;
     for(int i = 19; i >= 0; i--) {
          if(jump[a][i] != jump[b][i]) {
               a = jump[a][i];
               b = jump[b][i];
          }
     }
     return parent[a].first;
}
int goupfromnode(int a, int lcanode) {
     int maxval = 0;
     if(loc[a].first == loc[lcanode].first) {
          maxval = maxval + query(hldranges[loc[a].first].first + loc[a].second, hldranges[loc[a].first].first + loc[lcanode].second + 1);
     }else {
          // maxval = max(maxval, query(hldranges[loc[a].first].first + loc[a].second, hldranges[loc[a].first].second));
          // a = parent[hldpaths[loc[a].first][hldpaths[loc[a].first].size()-1]].first;
          while(loc[a].first != loc[lcanode].first) {
               maxval = maxval +  query(hldranges[loc[a].first].first + loc[a].second, hldranges[loc[a].first].second);
               a = parent[hldpaths[loc[a].first][hldpaths[loc[a].first].size()-1]].first;
          }
          maxval = maxval + query(hldranges[loc[a].first].first + loc[a].second, hldranges[loc[a].first].first + loc[lcanode].second + 1);
     }
     return maxval;
}
int querynode(int a, int b) {
     int lcanode = lca(a, b);
     return (goupfromnode(a, lcanode) + goupfromnode(b, lcanode));
}
const int maxval = 1e12;
struct line {
     int m, c;
     int operator()(int x) const {return m * x + c;};
};
struct lct {
     vector<int> children[2];
     vector<line> nodes;
     int newnode() {
          nodes.push_back({0, (int)1e16});
          children[0].push_back(-1);
          children[1].push_back(-1);
          return (int)nodes.size()-1;
     }
     int child(int node, int k) {
          if (children[k][node] == -1) {
               children[k][node] = newnode();
          }
          return children[k][node];
     }
     void insert(line add, int pos, int l, int r) {
          line cur = nodes[pos];
          int mid = (l + r) / 2;
          if (add(mid) < cur(mid)) {
               swap(cur, add);
               nodes[pos] = cur;
          }
          if (l + 1 == r) return;
          if (add(l) < cur(l)) {
               insert(add, child(pos, 0), l, mid);
          }else {
               insert(add, child(pos, 1), mid, r);
          }
     }
     int query(int x, int pos, int l, int r) {
          if (l + 1 == r) return nodes[pos](l);
          int mid = (l + r) / 2;
          if (x < mid) return min(nodes[pos](x), query(x, child(pos, 0), l, mid));
          return min(nodes[pos](x), query(x, child(pos, 1), mid, r));
     }
     void add(int m, int c) {
          line a = {m, c};
          insert(a, 0, 0, maxval);
     }
     int query(int x){return query(x, 0, 0, maxval);}
};
string sum(string a, string b) {
     string ans;
     reverse(a.begin(),a.end());
     reverse(b.begin(), b.end());
     int carry = 0;
     for(int i = 0; i < max(a.size(), b.size()); i++) {
          int total = carry;
          if(i < a.size()) {
               total += a[i] - '0';
          }
          if(i < b.size()) {
               total += b[i] - '0';
          }
          ans += total % 10 + '0';
          carry = total / 10;
     }
     if(carry == 1) ans += '1';
     reverse(ans.begin(), ans.end());
     return ans;
}
 
 
string times(string a, string b) {
     string total;
     for(int i = 0; i < a.size(); i++) {
          string s = b;
          for(int j = 0; j < a.size() - i - 1; j++) {
               s += '0';
          }
          for(int j = 0; j < a[i] - '0'; j++) {
               total = sum(total, s);
          }
     }
     return total;
}
int rdint() { // i put these in bc my code maybe tle, so reading in input fast will make it not tle!1!
     bool isneg = false;
     char c = getchar();
     while((c < '0' || c > '9')) {
          if(c == '-') {
               isneg = true;
          }
          c = getchar();
     }
     int a = 0;
     while(true) {
          a*=10;
          a+= c-'0';
          c = getchar();
          if(c < '0' || c > '9') break;
     }
     if(isneg) a *= -1;
     return a;
}
void prtint(int num) {
     if(num == 0) {
          putchar('0');
          putchar('\n');
          return;
     }
     if(num < 0) {
          num *= -1;
          putchar('-');
     }
     int val = 1000000000;
     while(num < val) {
          val /= 10;
     }
     while(val >= 1) {
          putchar('0' + (num / val) % 10);
          val /= 10;
     }
}
 
struct seg {
    bool set;
    signed setval;
    signed left;
    signed right;
    long long total;
    int l;
    int r;
    signed lazy;
    int max;
};
seg segment[20000005];
int pos = 1;
void propogate2(int node) {
    if(node == 1) {
        int a = 0;
    }
    if(segment[node].r - segment[node].l < 0) assert(1 == 2);
    if(pos >= 20000005) assert(1 == 2);
    //modify the tree such that this node has 2 children, no set and no lazyness.
    if(segment[node].r - segment[node].l != 1) {
        if(segment[node].left == -1) {
            seg newnode;
            newnode.set = segment[node].set;
            newnode.setval = segment[node].setval;
            newnode.left = -1;
            newnode.right = -1;
            newnode.total = 0;
            newnode.l = segment[node].l;
            newnode.r = (segment[node].l + segment[node].r)/2;
            newnode.lazy = 0;
            newnode.max = 0;
            segment[pos] = newnode;
            segment[node].left = pos;
            pos++;
        }
        if(segment[node].right == -1) {
            seg newnode;
            newnode.set = segment[node].set;
            newnode.setval = segment[node].setval;
            newnode.left = -1;
            newnode.right = -1;
            newnode.total = 0;
            newnode.l = (segment[node].l + segment[node].r)/2;
            newnode.r = segment[node].r;
            newnode.lazy = 0;
            newnode.max = 0;
            segment[pos] = newnode;
            segment[node].right = pos;
            pos++;
        }
    }
    if(segment[node].set) {
        segment[node].lazy = 0;
        long long val =((long long) (segment[node].r - segment[node].l)) * ((long long) segment[node].setval);
        if(val < 0) assert(1 == 2);
        segment[node].total = val;
        segment[node].max = segment[node].setval;
        segment[node].set = false;
        if(segment[node].r - segment[node].l != 1) {
            segment[segment[node].left].set = true;
            segment[segment[node].left].setval = segment[node].setval;
            segment[segment[node].right].set = true;
            segment[segment[node].right].setval = segment[node].setval;
        }
    }
    long long add = (long long) (((long long) (segment[node].lazy)) * (long long)(segment[node].r - segment[node].l));
    if(add < 0) assert(1 == 2);
    segment[node].total += add;
    segment[node].max += segment[node].lazy;
    if(segment[node].r - segment[node].l != 1) {
        if(segment[segment[node].left].set) {
            segment[segment[node].left].setval += segment[node].lazy;
        }else {
            segment[segment[node].left].lazy += segment[node].lazy;
        }
        if(segment[segment[node].right].set) {
            segment[segment[node].right].setval += segment[node].lazy;
        }else {
            segment[segment[node].right].lazy += segment[node].lazy;
        }
    }
    segment[node].lazy = 0;
    if(segment[node].total < 0) {
        assert(1 == 2);
    }
}
 
void propogate(int node) {
    propogate2(node);
    if(segment[node].r - segment[node].l != 1) {
        propogate2(segment[node].left);
        propogate2(segment[node].right);
    }
    propogate2(node);
    if(segment[node].r - segment[node].l != 1) {
        propogate2(segment[node].left);
        propogate2(segment[node].right);
        if(segment[segment[node].left].set)assert(1 == 2);
        if(segment[segment[node].left].lazy) assert(1 == 2);
        if(segment[segment[node].right].set) assert(1 == 2);
        if(segment[segment[node].right].lazy) assert(1 == 2);
    }
    if(segment[node].set) assert(1 == 2);
    if(segment[node].lazy) assert(1 == 2);
}
long long totalquery(int left, int right, int pos = 0) {
     if(left >= right) return 0;
    propogate(pos);
    int l = segment[pos].l;
    int r = segment[pos].r;
    if(l >= left && r <= right) {
        // if(segment[pos].total < 0) {
        //     assert(1 == 2);
        // }
        return segment[pos].total;
    }
    int mid = (l + r)/2;
    long long total = 0;
    if(left < mid) {
        total += totalquery(left, min(right, mid), segment[pos].left);
    }
    if(right > mid) {
        total += totalquery(max(left, mid), right, segment[pos].right);
    }
    return total;
}
int maxquery(int left, int right, int pos = 0) {
     if(left >= right) return 0;
    propogate(pos);
    int l = segment[pos].l;
    int r = segment[pos].r;
    if(l >= left && r <= right) {
        return segment[pos].max;
    }
    int mid = (l + r)/2;
    int maxval = 0;
    if(left < mid) {
        maxval = maxquery(left, min(right, mid), segment[pos].left);
    }
    if(right > mid) {
        maxval = max(maxval, maxquery(max(left, mid), right, segment[pos].right));
    }
    return maxval;
}
void addrange(int left, int right, int add, int pos = 0) {
     if(left >= right) return;
    propogate(pos);
    int l = segment[pos].l;
    int r = segment[pos].r;
    if(l >= left && r <= right) {
        segment[pos].lazy += add;
        propogate(pos);
        return;
    }
    int mid = (l + r) / 2;
    if(segment[pos].left==-1) assert(1 == 2);
    if(segment[pos].right==-1) assert(1 == 2);
    if(left < mid) {
        addrange(left, min(right, mid), add, segment[pos].left);
    }
    if(right > mid) {
        addrange(max(left, mid), right, add, segment[pos].right);
    }
    propogate(pos);
    if(!segment[pos].left) assert(1 == 2);
    if(segment[pos].r - segment[pos].l <= 1) assert(1 == 2);
    segment[pos].max = max(segment[segment[pos].left].max, segment[segment[pos].right].max);
    if(segment[segment[pos].left].total + segment[segment[pos].right].total < 0) assert(1 == 2);
    segment[pos].total = segment[segment[pos].left].total + segment[segment[pos].right].total;
}
void setrange(int left, int right, int setval, int pos = 0) {
     if(left >= right) return;
    propogate(pos);
    int l = segment[pos].l;
    int r = segment[pos].r;
    if(l >= left && r <= right) {
        segment[pos].set = true;
        segment[pos].setval = setval;
        propogate(pos);
        return;
    }
    int mid = (l + r) / 2;
    if(segment[pos].left==-1) assert(1 == 2);
    if(segment[pos].right==-1) assert(1 == 2);
    if(left < mid) {
        setrange(left, min(right, mid), setval, segment[pos].left);
    }
    if(right > mid) {
        setrange(max(left, mid), right, setval, segment[pos].right);
    }
    propogate(pos);
    if(segment[pos].r - segment[pos].l <= 1) assert(1 == 2);
    segment[pos].max = max(segment[segment[pos].left].max, segment[segment[pos].right].max);
    if(segment[segment[pos].left].total + segment[segment[pos].right].total < 0) assert(1 == 2);
    segment[pos].total = segment[segment[pos].left].total + segment[segment[pos].right].total;
}
const int SQRT = 400;
bool cmp(pair<int, pair<int, int>> a, pair<int, pair<int, int>> b) {
     if(a.second.first / SQRT > b.second.first / SQRT) {
          return false;
     }else if(a.second.first / SQRT < b.second.first / SQRT) {
          return true;
     }else {
          return a.second.second < b.second.second;
     }
}
signed main() {
     segment[0] = {false, 0, -1, -1, 0, 0, 4000000005, 0, 0};
     propogate(0);
     ios_base::sync_with_stdio(false);
     cin.tie(nullptr);
     cout.tie(nullptr);
     n = 3;
     int a = rdint(), b = rdint();
     neighbors[1].push_back(2);
     neighbors[1].push_back(3);
     root(1, 0);
     for(int i = 1; i <= n; i++) {
          for(int j = 0; j < children[i].size(); j++) {
               if(subsum[children[i][j].first] > (subsum[i]-1)/2) {
                    children[i][j].second = true;
                    parent[children[i][j].first].second = true;
               }else {
                    children[i][j].second = false;
                    parent[children[i][j].first].second = false;
               }
          }
     }
     queue<int> nodes;
     for(int i = 1; i <= n; i++) {
          if(children[i].empty())nodes.push(i);
     }
     for(int i = 1; i <= n; i++) numchildren[i] = children[i].size();
     bitset<200005> inpaths;
     while(!nodes.empty()) {
          int node = nodes.front();
          nodes.pop();
          vector<int> path;
          path.push_back(node);
          inpaths[node]=true;
          loc[node] = {hldpaths.size(), 0};
          while(node != 1 && parent[node].second) {
               node = parent[node].first;
               loc[node] = {hldpaths.size(), path.size()};
               path.push_back(node);
               inpaths[node] = true;
          }
          if(node != 1) {
               node = parent[node].first;
               numchildren[node]--;
               if(numchildren[node] == 0 && !inpaths[(node)]) {
                    nodes.push(node);
               }
          }
          hldpaths.push_back(path);
          if(hldranges.empty()) {
               hldranges.push_back({0, path.size()});
          }else {
               hldranges.push_back({hldranges[hldranges.size() -1].second, hldranges[hldranges.size() -1].second + path.size()});
          }
     }
     calc();
     change(2, a);
     change(3, b);
     arr[67] = {querynode(1, 2), 1, Rand(), querynode(1, 2), NULL, NULL};
     arr[41] = {querynode(1, 3), 1, Rand(), querynode(1, 3), NULL, NULL};
     treap * ans = merge(arr + 67, arr + 41);
     lct tr;
     tr.newnode();
     tr.add(ans->total / 2, (ans->total + 1) / 2);
     addrange(0, tr.query(1) * 2, 1);
     setrange(0, tr.query(1), 0);
     vector<int> arr = {totalquery(0, tr.query(1)), totalquery(tr.query(1), tr.query(1) * 2)};
     int l = 0, r = 0;
     vector<pair<int, pair<int, int>>> v;
     for(int i = 0; i < 1; i++) {
          int ql, qr;
          ql = 0, qr = 1;
          v.push_back({i, {ql, qr}});
     }
     sort(v.begin(), v.end(), cmp);
     unordered_map<int, int> ma;
     ma[arr[0]] = 1;
     int asad[2];
     int total = 0;
     for(int i = 0; i < 1;i++) {
          while(r < v[i].second.second) {
               r++;
               total += arr[r];
               ma[arr[r]]++;
          }
          while(l > v[i].second.first) {
               l--;
               total += arr[l];
               ma[arr[l]]++;
          }
          while(r > v[i].second.second) {
               ma[arr[r]] --;
               total -= arr[r];
               r--;
          }
          while(l < v[i].second.first) {
               ma[arr[l]] --;
               total -= arr[l];
               l++;
          }
          asad[v[i].first] = total;
     }
     for(int i = 0; i < 1; i++) {
          prtint(asad[i]);
     }
     putchar('\n');
     return 0;
}
