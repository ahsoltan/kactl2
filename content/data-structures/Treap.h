/**
 * Author: someone on Codeforces
 * Date: 2017-03-14
 * Source: folklore
 * Description: A short self-balancing tree. It acts as a
 *  sequential container with log-time splits/joins, and
 *  is easy to augment with additional data.
 * Time: $O(\log N)$
 * Status: stress-tested
 */
#pragma once

mt19937 rng(2137);
struct Node {
	Node *l = 0, *r = 0, *p = 0;
	int val, y, c = 1;
	Node(int val) : val(val), y(rng()) {}
	friend int cnt(Node* n) { return n ? n->c : 0; }
	void recalc() { c = cnt(l) + cnt(r) + 1; }
	void push() {}
};

void each(Node* n, auto f) {
	if (n) { each(n->l, f); f(n->val); each(n->r, f); }
}

pair<Node*, Node*> split(Node* n, int k) {
	if (!n) return {};
	n->push(); n->p = 0;
	if (cnt(n->l) >= k) { // "n->val >= k" for lower_bound(k)
		auto pa = split(n->l, k);
		n->l = pa.second;
		n->recalc();
		return {pa.first, n};
	} else {
		auto pa = split(n->r, k - cnt(n->l) - 1); // and just "k"
		n->r = pa.first;
		n->recalc();
		return {n, pa.second};
	}
}

Node* merge(Node* l, Node* r) {
	if (!l || !r) return l ?: r;
	if (l->y > r->y) {
		l->push();
		l->r = merge(l->r, r);
		if (l->r) l->r->p = l;
		l->recalc();
		return l;
	} else {
		r->push();
		r->l = merge(l, r->l);
		if (r->l) r->l->p = r;
		r->recalc();
		return r;
	}
}

Node* ins(Node* t, Node* n, int pos) {
	auto [l,r] = split(t, pos);
	return merge(merge(l, n), r);
}

// Union of two sorted treaps, O(m log(n/m)) where m<=n
// Makes small-to-large O(n log n) instead of log^2.
// Requires lower_bound split (not the default one).
Node* unite(Node* a, Node* b) {
	if (!a || !b) return a ?: b;
	if (a->y < b->y) swap(a, b);
	auto [l, r] = split(b, a->val); // lower_bound split
	return merge(unite(l,a->l), merge(a, unite(r,a->r)));
}

// Number of elements before n. If there are range
// reverse queries, recursively push the path to n.
int idx(Node* n) {
	int c = cnt(n->l);
	while (n->p) {
		if (n->p->l != n) c += cnt(n->p->l) + 1;
		n = n->p;
	}
	return c;
}

// Example application: move the range [l, r) to index k
void move(Node*& t, int l, int r, int k) {
	Node *a, *b, *c;
	tie(a,b) = split(t, l); tie(b,c) = split(b, r - l);
	if (k <= l) t = merge(ins(a, b, k), c);
	else t = merge(a, ins(c, b, k - r));
}
