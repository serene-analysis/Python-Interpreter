#include "int2048.h"
namespace sjtu{
// your hpp source code
const int Maxn=(1<<21)+5;
const int base=1e9;
const int mod[3]={(1<<21)*470+1,(1<<21)*476+1,(1<<21)*479+1};
const int g=3;
typedef __int128 i128;
i128 qpow(i128 a, i128 b, i128 mod){
    i128 ans = 1;
    a %= mod;
    while(b){
        if(b&1)ans = ans * a % mod;
        a = a * a % mod;
        b >>= 1;
    }
    return ans;
}
i128 smul(i128 a, i128 b, i128 mod){
    i128 ans = 0;
    while(b){
        if(b&1)ans = (ans + a) % mod;
        a = (a + a) % mod;
        b >>= 1;
    }
    return ans;
}
int rev[Maxn];
void rev_init(int n, int cou){
    for(int i=0;i<n;i++){
        rev[i] = (rev[i>>1] >> 1) | ((i & 1) << (cou - 1));
    }
    return;
}
int *gp[25],*gip[25],mem[Maxn*4],*pt = mem;
void g_init(int lgN, const int mod){
    pt = mem;
    //std::cerr << lgN << '\n';
    for(int i=1;i<=lgN;i++){
        int fv = qpow(g, (mod - 1) / (1 << i), mod), sv = qpow((mod + 1) / g, (mod - 1) / (1 << i), mod);
        gp[i] = pt, pt += (1 << i);
        gip[i] = pt, pt += (1 << i);
        gp[i][0] = gip[i][0] = 1;
        for(int j=1;j<(1<<i);j++){
            gp[i][j] = 1ll * gp[i][j-1] * fv % mod;
            gip[i][j] = 1ll * gip[i][j-1] * sv % mod;
        }
    }
    return;
}
void int2048::sizen(int x){
    pol.resize(x);
    return;
}
int& int2048::operator[](int x){ return pol.at(x); }
inline int inc(int x, int y, const int mod){ return x + y < mod ? x + y : x + y - mod; }
inline int dec(int x, int y, const int mod){ return x >= y? x - y : x - y + mod; }
void int2048::ntt(int n, int type, const int mod){
    for(int i=0;i<n;i++){
        if(i < rev[i])std::swap(pol[i],pol[rev[i]]);
    }
    int ncn = 1;
    for(int mid=1;mid<n;mid<<=1,ncn++){
        int nr = mid * 2;
        for(int i=0;i<n;i+=nr){
            int *v = (type == 1 ? gp : gip)[ncn];
            for(int j=0;j<mid;j++){
                int lef = pol[i+j], rig = 1ll * v[j] * pol[i+j+mid] % mod;
                pol[i+j]=inc(lef, rig, mod), pol[i+j+mid]=dec(lef, rig, mod);
            }
        }
    }
    return;
}
int2048 int2048::mul(int2048 now, int2048 oth, const int mod){
    int n = now.pol.size(), m = oth.pol.size();
    int2048 ret;
    ret.sizen(n + m - 1);
    int nlen = std::max(n, m);
    if(nlen <= 16){
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                ret[i+j] = inc(ret[i+j], 1ll * now[i] * oth[j] % mod, mod);
            }
        }
        return ret;
    }
    m = n + m - 1, n = 1;
    int cou = 0;
    while((1 << cou) <= 2 * nlen)cou++, n <<= 1;
    g_init(cou, mod);
    rev_init(n, cou);
    now.sizen(n), oth.sizen(n);
    now.ntt(n, 1, mod), oth.ntt(n, 1, mod);
    for(int i=0;i<n;i++)now[i] = 1ll * now[i] * oth[i] % mod;
    now.ntt(n, -1, mod);
    int ninv = qpow(n, mod - 2, mod);
    for(int i=0;i<m;i++)ret[i] = 1ll * now[i] * ninv % mod;
    return ret;
}
int2048::int2048(){
    pol.resize(1), pol[0] = 0, neg = false;
    return;
}
int2048::int2048(bool y){
    neg = false;
    sizen(1);
    if(y){
        pol[0] = 1;
    }
    else{
        pol[0] = 0;
    }
    return;
}
int2048::int2048(int y){
    i128 x = y;
    neg = false;
    if(x < 0){
        neg = true;
        x = -x;
    }
    if(x < base){
        sizen(1);
        pol[0] = x;
    }
    else if(x < 1ll * base * base){
        sizen(2);
        pol[0] = x % base, pol[1] = x / base;
    }
    else{
        sizen(3);
        pol[0] = x % base, pol[1] = (x / base) % base, pol[2] = x / base / base;
    }
    assert(pol.back() || pol.size() == 1);
    return;
}
int2048::int2048(long long y){
    i128 x = y;
    neg = false;
    if(x < 0){
        neg = true;
        x = -x;
    }
    if(x < base){
        sizen(1);
        pol[0] = x;
    }
    else if(x < 1ll * base * base){
        sizen(2);
        pol[0] = x % base, pol[1] = x / base;
    }
    else{
        sizen(3);
        pol[0] = x % base, pol[1] = (x / base) % base, pol[2] = x / base / base;
    }
    assert(pol.back() || pol.size() == 1);
    return;
}
#include<cassert>
int2048::int2048(const std::string &str){
    assert(!str.empty());
    pol.clear();
    neg = false;
    std::string now = str;
    if(str[0] == '-'){
        neg = true;
        now.clear();
        for(auto dt = std::next(str.begin());dt!=str.end();dt++){
            now += *dt;
            assert(*dt >= '0' && *dt <= '9');
        }
    }
    assert(!now.empty());
    if(now[0] == '0'){
        auto dt = now.begin();
        while(dt != now.end() && *dt == '0'){
            dt++;
        }
        now.assign(dt, now.end());
        if(now.empty()){
            *this = int2048(0ll);
            return;
        }
    }
    //std::cout << "? " << now << '\n';
    int nsi = now.length(), cnt = 0;
    sizen((nsi - 1) / 9 + 1);
    for(int i=nsi-1;i>=0;i-=9){
        int x = 0;
        for(int j=std::max(i-8,0);j<nsi&&j<=i;j++){
            x = x * 10 + now[j] - '0';
            assert(now[j] >= '0' && now[j] <= '9');
        }
        pol[cnt++] = x;
        //std::cout << "x=" << x << '\n';
    }
    if(neg && pol.size() == 1 && pol[0] == 0)neg = false;
    assert(pol.back() || pol.size() == 1);
    //std::cout << "string_construct:pol.size() = " << pol.size() << '\n';
    //for(int i=pol.size()-1;i>=0;i--){
    //    int x = pol[i];
    //    std::cout << "x = " << x << "???\n";
    //}
    return;
}
int2048::int2048(const int2048 &x){
    pol = x.pol, neg = x.neg;
    //assert(pol.back() || pol.size() == 1);
    //while(pol.size() > 1 && pol.back() == 0){
    //    pol.pop_back();
    //    assert(false);
    //}
    return;
}
double int2048::toDouble(){
    std::string str = toString();
    int nsi = str.length();
    double ret = 0;
    for(int i=0;i<nsi;i++){
        if(str[i] == '-'){
            ret *= -1;
        }
        else{
            ret = ret * 10 + str[i] - '0';
        }
    }
    return ret;
}
std::string int2048::toString(){
    std::string ret;
    int2048 now = *this;
    if(now.neg && now.pol.size() == 1 && now[0] == 0){
        now.neg = false;
        assert(false);
    }
    //if(now.pol.size() == 1 && now[0] == 0){
    //    assert(false);
    //}
    std::ios::sync_with_stdio(false);
    if(now.neg){
        ret += '-';
    }
    assert(now.pol.back() || now.pol.size() == 1);
    //if(now.pol.size() == 0){
    //    std::cout << "shik\n";
    //}
    int size = now.pol.size();
    for(int i=size-1;i>=0;i--){
        int x = now.pol[i];
        assert(x >= 0);
        std::vector<int>out;
        if(i == size - 1){
            while(x){
                out.push_back(x % 10), x /= 10;
            }
        }
        else{
            for(int j=0;j<=8;j++)out.push_back(x % 10), x /= 10;
        }
        //std::cout << "out.size() = " << out.size() << '\n';
        for(int j=8;j>=0;j--)ret += out[j] + '0';
    }
    return ret;
}
void int2048::read(const std::string &str){
    *this = int2048(str);
    assert(pol.back() || pol.size() == 1);
    return;
}
void int2048::print(){
    //std::cout << "print:pol.size() = " << pol.size() << '\n';
    if(neg && pol.size() == 1 && pol[0] == 0)neg = false;
    if(neg){
        std::cout << '-';
    }
    assert(pol.back() || pol.size() == 1);
    int size = pol.size();
    std::cout << pol.back();
    //std::cout << "?";
    for(int i=size-2;i>=0;i--){
        int x = pol[i];
        assert(x >= 0);
        //std::cout << "x = " << x << "???\n";
        std::vector<int>out;
        for(int j=0;j<=8;j++)out.push_back(x % 10), x /= 10;
        for(int j=8;j>=0;j--)std::cout<<out[j];
        //std::cout << "??";
    }
    return;
}
bool operator==(const int2048 &inow, const int2048 &ioth){
    assert(inow.pol.back() || inow.pol.size() == 1);
    assert(ioth.pol.back() || ioth.pol.size() == 1);
    int2048 now = inow;
    if(now.neg && now.pol.size() == 1 && now[0] == 0)now.neg = false;
    int2048 oth = ioth;
    if(oth.neg && oth.pol.size() == 1 && oth[0] == 0)oth.neg = false;
    if(now.neg != oth.neg || now.pol.size() != oth.pol.size()){
        return false;
    }
    int nsi = now.pol.size();
    for(int i=0;i<nsi;i++){
        assert(now[i] < base && oth[i] < base);
        if(now.pol[i] != oth.pol[i]){
            return false;
        }
    }
    return true;
}
bool operator!=(const int2048 &now, const int2048 &oth){
    assert(now.pol.back() || now.pol.size() == 1);
    assert(oth.pol.back() || oth.pol.size() == 1);
    return !(now == oth);
}
bool operator<(const int2048 &inow, const int2048 &ioth){
    //assert(inow.pol.back() || inow.pol.size() == 1);
    //assert(ioth.pol.back() || ioth.pol.size() == 1);
    int2048 now = inow;
    if(now.neg && now.pol.size() == 1 && now[0] == 0)now.neg = false;
    int2048 oth = ioth;
    if(oth.neg && oth.pol.size() == 1 && oth[0] == 0)oth.neg = false;
    if(now.neg != oth.neg){
        return now.neg;
    }
    if(now.neg){
        if(now.pol.size() != oth.pol.size()){
            return now.pol.size() > oth.pol.size();
        }
        int nsi = now.pol.size();
        for(int i=nsi-1;i>=0;i--){
            if(now.pol[i] != oth.pol[i]){
                return now.pol[i] > oth.pol[i];
            }
        }
        return false;
    }
    else{
        if(now.pol.size() != oth.pol.size()){
            return now.pol.size() < oth.pol.size();
        }
        int nsi = now.pol.size();
        for(int i=nsi-1;i>=0;i--){
            if(now.pol[i] != oth.pol[i]){
                return now.pol[i] < oth.pol[i];
            }
        }
        return false;
    }
}
bool operator>(const int2048 &inow, const int2048 &ioth){
    return ioth < inow;
}
bool operator<=(const int2048 &now, const int2048 &oth){
    return !(now > oth);
}
bool operator>=(const int2048 &now, const int2048 &oth){
    return !(now < oth);
}
int2048 int2048::operator+()const{
    int2048 ret = *this;
    assert(pol.back() || pol.size() == 1);
    if(ret.neg && pol.size() == 1 && pol[0] == 0){
        ret.neg = false;
        assert(0);
    }
    //assert(pol.back() || pol.size() == 1);
    return ret;
}
int2048 int2048::operator-()const{
    int2048 ret = *this;
    ret.neg = !ret.neg;
    bool nzero = false;
    for(int v : pol){
        nzero = nzero || (v > 0);
    }
    if(!nzero){
        //assert(pol.size() == 1);
    }
    assert(pol.back() || pol.size() == 1);
    if(pol.size() == 1 && pol[0] == 0)ret.neg = false;
    return ret;
}
int2048 add(int2048 now, const int2048 &ioth){
    if(now.neg && now.pol.size() == 1 && now[0] == 0)now.neg = false;
    int2048 oth = ioth;
    if(oth.neg && oth.pol.size() == 1 && oth[0] == 0)oth.neg = false;
    assert(now.pol.back() || now.pol.size() == 1);
    assert(oth.pol.back() || oth.pol.size() == 1);
    //std::cout << "?" <<'\n';
    //std::cout << "add:" << now << "," << oth << '\n';
    if(now < -oth){
        int2048 ret = ((-oth).add(-now));
        ret.neg = !ret.neg;
        assert(ret.neg);
        assert(ret.pol.back() || ret.pol.size() == 1);
        return ret;
    }
    if(!oth.neg && now.neg){
        int2048 ret = add(oth, now);
        assert(ret.pol.back() || ret.pol.size() == 1);
        return ret;
    }
    if(oth.neg){
        int nsi = now.pol.size(), osi = oth.pol.size();
        //if(nsi < osi)std::cout<<"????????";
        for(int i=0;i<osi;i++){
            now[i] -= oth[i];
        }
        for(int i=0;i<nsi;i++){
            if(now[i] < 0){
                now[i] += base;
                assert(now[i] >= 0);
                now[i+1]--;
            }
        }
        while(nsi != 1 && now[nsi-1] == 0)nsi--;
        now.sizen(nsi);
        assert(now.pol.back() || now.pol.size() == 1);
        //while(now.pol.size() > 1 && now.pol.back() == 0){
        //    now.pol.pop_back();
        //    assert(false);
        //}
        if(now.neg && now.pol.size() == 1 && now[0] == 0)now.neg = false;
        assert(!now.neg);
        return now;
    }
    else{
        int nsi = now.pol.size(), osi = oth.pol.size(), msi = std::max(nsi,osi) + 1;
        now.sizen(msi);
        //for(int i=nsi;i<msi;i++)if(now[i])std::cout << "???\n";
        for(int i=0;i<osi;i++){
            now[i] += oth[i];
        }
        for(int i=0;i<msi;i++){
            if(now[i] >= base){
                now[i] -= base;
                now[i+1]++;
            }
        }
        while(msi != 1 && now[msi-1] == 0)msi--;
        now.sizen(msi);
        assert(now.pol.back() || now.pol.size() == 1);
        //while(now.pol.size() > 1 && now.pol.back() == 0){
        //    now.pol.pop_back();
        //    assert(false);
        //}
        assert(!now.neg);
        //std::cout << "now:" << now << '\n';
        return now;
    }
}
int2048& int2048::add(const int2048 &oth){
    return *this = sjtu::add(*this, oth);
}
int2048 minus(int2048 now, const int2048 &ioth){
    return add(now, -ioth);
}
int2048& int2048::minus(const int2048 &oth){
    return *this = sjtu::minus(*this, oth);
}
int2048& int2048::operator=(const int2048 &oth){
    if(this == &oth){
        return *this;
    }
    pol = oth.pol, neg = oth.neg;
    return *this;
}
int2048 operator+(int2048 now, const int2048 &oth){
    return add(now, oth);
}
int2048& int2048::operator+=(const int2048 &oth){
    add(oth);
    return *this;
}
int2048 operator-(int2048 now, const int2048 &oth){
    return minus(now, oth);
}
int2048& int2048::operator-=(const int2048 &oth){
    minus(oth);
    return *this;
}
int2048 operator*(int2048 now, const int2048 &oth){
    //assert(false);
    int2048 oans = int2048().mul(now, oth, mod[0]), sans = int2048().mul(now, oth, mod[1]), tans = int2048().mul(now, oth, mod[2]);
    int2048 ret;
    int n = now.pol.size(), m = oth.pol.size();
    ret.sizen(n + m - 1);
    i128 amod=(i128)(mod[0]) * mod[1] * mod[2];
    i128 ft=smul((i128)(mod[1]) * mod[2],(i128)(qpow(mod[1],mod[0] - 2,mod[0])) * qpow(mod[2],mod[0] - 2,mod[0]),amod),
        st=smul((i128)(mod[0]) * mod[2],(i128)(qpow(mod[0],mod[1] - 2,mod[1])) * qpow(mod[2],mod[1] - 2,mod[1]),amod),
        tt=smul((i128)(mod[0]) * mod[1],(i128)(qpow(mod[0],mod[2] - 2,mod[2])) * qpow(mod[1],mod[2] - 2,mod[2]),amod);
    i128 got = 0;
    for(int i=0;i<n+m-1;i++){
        i128 nv = (oans[i] * ft + sans[i] * st + tans[i] * tt) % amod;
        got = (got + nv) % amod;
        ret[i] = got % base;
        got /= base;
    }
    while(got){
        ret.pol.push_back(got % base);
        got /= base;
    }
    ret.neg = (now.neg != oth.neg);
    while(ret.pol.back() == 0 && ret.pol.size() > 1){
        ret.pol.pop_back();
    }
    if(ret.pol.size() == 1 && ret.pol.back() == 0){
        ret.neg = false;
    }
    return ret;
}
int2048& int2048::operator*=(const int2048 &oth){
    *this = (*this) * oth;
    return *this;
}
int2048 lmove(int2048 now, int k){
    now.pol.insert(now.pol.begin(), k, 0);
    return now;
}
int2048 rmove(int2048 now, int k){
    int2048 ret = 0;
    ret.neg = now.neg;
    if(k >= now.pol.size()){
        ret.neg = false;
        return ret;
    }
    ret.pol.assign(now.pol.begin() + k, now.pol.end());
    return ret;
}
int2048 div(int2048 now, int k){
    int nsi = now.pol.size(), tag = 0;
    for(int i=nsi-1;i>=0;i--){
        now.pol[i] += tag * base;
        tag = now.pol[i] % k, now.pol[i] /= k;
    }
    //std::cout << "nsi = " << nsi << '\n';
    while(nsi != 1 && now.pol[nsi-1] == 0)nsi--;
    now.sizen(nsi);
    return now;
}
int2048 div(int2048 now, int2048 oth){
    int2048 lef = -now, rig = now, ans = int2048(0ll);
    //std::cout << "now = " << now << '\n' << "oth = " << oth << '\n';
    while(lef <= rig){
        int2048 mid = sjtu::div(lef + rig, 2);
        if(mid * oth <= now){
            ans = mid;
            lef = mid + 1;
        }
        else{
            rig = mid - 1;
        }
    }
    //std::cout << "ok\n";
    return ans;
}
int2048 int2048::newton_inv(int n){
    //std::cout << "n = " << n << ", num = " << *this << '\n';
    int size = pol.size();
    if (n <= 16) {
        int2048 up = 1;
        up = lmove(up, n);
        return div(up, *this);
    }
    int k = (n - size + 2) >> 1, dlt = k > size ? 0 : size - k;
    int2048 dw = rmove(*this, dlt);
    int nsize = k + dw.pol.size();
    int2048 ret = dw.newton_inv(nsize), lef = ret + ret, rig = (*this) * ret * ret;
    //std::cout << "end : n = " << n << ", num = " << *this << '\n';
    return lmove(lef, n - nsize - dlt) - rmove(rig, 2 * (nsize + dlt) - n) - 1;
}
int2048 operator/(int2048 now, const int2048 &ioth){
    //assert(false);
    if(now.neg && now.pol.size() == 1 && now[0] == 0)now.neg = false;
    int2048 oth = ioth;
    if(oth.neg && oth.pol.size() == 1 && oth[0] == 0){
        oth.neg = false;
        assert(false);
    }
    if(now.neg && oth.neg){
        return (-now) / (-oth);
    }
    if(now.neg){
        int2048 ret = -(-now / oth);
        if(ret * ioth != now){
            return ret - 1;
        }
        return ret;
    }
    if(oth.neg){
        int2048 ret = -(now / -oth);
        if(ret * ioth != now){
            return ret - 1;
        }
        return ret;
    }
    if(now < oth){
        return 0;
    }
    //std::cout << "div:" << now << "......\n" << oth << '\n';
    int k = now.pol.size() - oth.pol.size() + 2, dlt = k > oth.pol.size() ? 0 : oth.pol.size() - k;
    int2048 dw = rmove(oth, dlt);
    if (dlt != 0) dw += 1;
    int nsize = k + dw.pol.size();
    int2048 u = now * dw.newton_inv(nsize);
    int2048 ret = rmove(u, nsize + dlt), left = now - ret * oth;
    //std::cout << "left = " << left << '\n' << "oth = " << oth << '\n';
    while (left >= oth){
        ret += 1;
        left -= oth;
    }
    while(ret.pol.size() > 1 && ret.pol.back() == 0){
        ret.pol.pop_back();
    }
    return ret;
}// get b'=[10^{2n}/b],get [ab'/10^{2n}], adjust.
int2048& int2048::operator/=(const int2048 &oth){
    *this = (*this) / oth;
    return *this;
}
int2048 operator%(int2048 now, const int2048 &oth){
    return now - now / oth * oth;
}
int2048& int2048::operator%=(const int2048 &oth){
    *this = (*this) % oth;
    return *this;
}
std::istream &operator>>(std::istream &is, int2048 &now){
    std::string str;
    is>>str;
    now.read(str);
    return is;
}
std::ostream &operator<<(std::ostream &os, const int2048 &inow){
    int2048 now = inow;
    if(now.neg && now.pol.size() == 1 && now[0] == 0){
        now.neg = false;
        assert(false);
    }
    //if(now.pol.size() == 1 && now[0] == 0){
    //    assert(false);
    //}
    std::ios::sync_with_stdio(false);
    if(now.neg){
        os << '-';
    }
    assert(now.pol.back() || now.pol.size() == 1);
    //if(now.pol.size() == 0){
    //    std::cout << "shik\n";
    //}
    int size = now.pol.size();
    os << now.pol.back();
    for(int i=size-2;i>=0;i--){
        int x = now.pol[i];
        assert(x >= 0);
        std::vector<int>out;
        for(int j=0;j<=8;j++)out.push_back(x % 10), x /= 10;
        //std::cout << "out.size() = " << out.size() << '\n';
        for(int j=8;j>=0;j--)os << out[j];
    }
    return os;
}
}