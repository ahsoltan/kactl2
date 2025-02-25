/**
 * Author: Krzysztof Potepa
 * Date: 2024
 * License: N/A
 * Description: Extended Li Chao tree (segment tree for functions). Let F be a family of functions closed under function addition, such that for every $f \neq g$ from the family F there exists x such that $f(z) \leq g(z)$ for $z \leq x$ else $f(z) \geq g(z)$ or
 the other way around (intersect at one point). Typically F is the family of linear functions. DS maintains a sequence $c_0, c_1 \ldots c_{n - 1}$ under operations max, add.
 * Status: Tested on random stuff
 */
#pragma once

struct LiChao {
	struct Func {
		ll a, b; // a*x + b
		// Evaluate function in point x
		ll operator()(ll x) const { return a*x+b; }
		Func operator+(Func r) const {
			return {a+r.a, b+r.b};
		} // Sum of two functions
	}; // ID_ADD/MAX neutral elements for add/max
	static constexpr Func ID_ADD{0, 0};
	static constexpr Func ID_MAX{0, ll(-1e9)};
	vector<Func> val, lazy;
	int len;
	// Initialize tree for n elements; time: O(n)
	LiChao(int n = 0) {
		for (len = 1; len < n; len *= 2);
		val.resize(len*2, ID_MAX);
		lazy.resize(len*2, ID_ADD);
	}
	void push(int i) {
		if (i < len) rep(j, 2) {
			lazy[i*2+j] = lazy[i*2+j] + lazy[i];
			val[i*2+j] = val[i*2+j] + lazy[i];
		}
		lazy[i] = ID_ADD;
	} // For each x in [vb;ve)
	  // set c[x] = max(c[x], f(x));
	  // time: O(log^2 n) in general case,
	  //       O(log n) if [vb;ve) = [0;len)
	void max(int vb, int ve, Func f,
	         int i = 1, int b = 0, int e = -1) {
		if (e < 0) e = len;
		if (vb >= e || b >= ve || i >= len*2)
			return;
		int m = (b+e) / 2;
		push(i);
		if (b >= vb && e <= ve) {
			auto& g = val[i];
			if (g(m) < f(m)) swap(g, f);
			if (g(b) < f(b))
				max(vb, ve, f, i*2, b, m);
			else
				max(vb, ve, f, i*2+1, m, e);
		} else {
			max(vb, ve, f, i*2, b, m);
			max(vb, ve, f, i*2+1, m, e);
		}
	} // For each x in [vb;ve)
	  // set c[x] = c[x] + f(x);
	  // time: O(log^2 n) in general case,
	  //       O(1) if [vb;ve) = [0;len)
	void add(int vb, int ve, Func f,
	         int i = 1, int b = 0, int e = -1) {
		if (e < 0) e = len;
		if (vb >= e || b >= ve) return;
		if (b >= vb && e <= ve) {
			lazy[i] = lazy[i] + f;
			val[i] = val[i] + f;
		} else {
			int m = (b+e) / 2;
			push(i);
			max(b, m, val[i], i*2, b, m);
			max(m, e, val[i], i*2+1, m, e);
			val[i] = ID_MAX;
			add(vb, ve, f, i*2, b, m);
			add(vb, ve, f, i*2+1, m, e);
		}
	} // Get value of c[x]; time: O(log n)
	auto query(int x) {
		int i = x+len;
		auto ret = val[i](x);
		while (i /= 2)
			ret = ::max(ret+lazy[i](x), val[i](x));
		return ret; } };
