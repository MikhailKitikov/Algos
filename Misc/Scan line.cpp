#include <iostream>
#include <vector>
#include <algorithm>

struct Segment {
    int x_left;
    int x_right;
    int y_coord;

    Segment(int y_down, int y_up, int x_coord) :
        x_left(y_down), x_right(y_up), y_coord(x_coord) {
    }
};

struct TreeNode {
    Segment node_seg;
    int height;
    TreeNode* left;
    TreeNode* right;

    TreeNode(Segment node_seg, int height, TreeNode* left, TreeNode* right) :
        node_seg(node_seg), height(height), left(left), right(right) {
    }
};

class AVLtree {
public:
    AVLtree() : root(nullptr) {
    }

    int InsertSegment(Segment seg) {
        return InsertSegment(root, seg);
    }

    void RemoveSegment(Segment seg) {
        root = RemoveSegment(seg, root);
    }    

    void ClearSubtree() {
        ClearSubtree(root);
        root = nullptr;
    }

private:
    TreeNode* root;

    int InsertSegment(TreeNode* &node, Segment seg) {
        int ans = 0;
        if (!node) {
            node = new TreeNode(seg, 0, nullptr, nullptr);
            ans = 1;
        } else if (seg.x_right < node->node_seg.x_right && 
            seg.x_left > node->node_seg.x_left) {
            return ans;
        } else if (seg.x_right == node->node_seg.x_right && seg.x_left == node->node_seg.x_left) {
            ans = -1;
            return ans;
        } else if (seg.x_right < node->node_seg.x_right) {
            ans = InsertSegment(node->left, seg);
            if (Overweight(node)) {
                if (seg.x_right < node->left->node_seg.x_right) {
                    node = ShortRightRotate(node);
                } else {
                    node = LongRightRotate(node);
                }
            }
        } else if (seg.x_right > node->node_seg.x_right) {
            ans = InsertSegment(node->right, seg);
            if (Overweight(node)) {
                if (seg.x_right > node->right->node_seg.x_right) {
                    node = ShortLeftRotate(node);
                } else {
                    node = LongLeftRotate(node);
                }
            }
        }
        RecalcHeight(node);
        return ans;
    }

    void ClearSubtree(TreeNode* node) {
        if (!node) {
            return;
        }
        ClearSubtree(node->left);
        ClearSubtree(node->right);
        delete node;
    }

    TreeNode* ShortRightRotate(TreeNode* &node) {
        if (!node->left) {
            return node;
        }        
        TreeNode* temp_node = node->left;
        node->left = temp_node->right;
        temp_node->right = node;
        RecalcHeight(node);
        RecalcHeight(temp_node);
        return temp_node;
    }

    TreeNode* ShortLeftRotate(TreeNode* &node) {
        if (!node->right) {
            return node;
        }        
        TreeNode* temp_node = node->right;
        node->right = temp_node->left;
        temp_node->left = node;
        RecalcHeight(node);
        RecalcHeight(temp_node);
        return temp_node;
    }

    TreeNode* LongRightRotate(TreeNode* &node) {
        node->left = ShortLeftRotate(node->left);
        return ShortRightRotate(node);
    }

    TreeNode* LongLeftRotate(TreeNode* &node) {
        node->right = ShortRightRotate(node->right);
        return ShortLeftRotate(node);
    }

    TreeNode* findMin(TreeNode* node) {
        if (!node) {
            return nullptr;
        }
        return (node->left ? findMin(node->left) : node);
    }

    TreeNode* RemoveSegment(Segment segm, TreeNode* node) {
        if (!node) {
            return nullptr;
        } else if (segm.x_right < node->node_seg.x_right) {
            node->left = RemoveSegment(segm, node->left);
        } else if (segm.x_right > node->node_seg.x_right) {
            node->right = RemoveSegment(segm, node->right);
        } else if (node->left != nullptr && node->right != nullptr) {
            node->node_seg = findMin(node->right)->node_seg;
            node->right = RemoveSegment(node->node_seg, node->right);
        } else {
            TreeNode* temp_node = node;
            if (node->left == NULL) {
                node = node->right;
            } else {
                node = node->left;
            }
            delete temp_node;
        }
        if (!node) {
            return node;
        }

        RecalcHeight(node);
        if (Overweight(node) == -1) {
            if (Overweight(node->right) == -1) {
                return ShortLeftRotate(node);
            } else {
                return LongLeftRotate(node);
            }
        } else if (Overweight(node) == 1) {
            if (Overweight(node->left) == 1) {
                return ShortRightRotate(node);
            } else {
                return LongRightRotate(node);
            }
        }
        return node;
    }

    int GetHeight(TreeNode* node) {
        return (node == NULL ? -1 : node->height);
    }

    int Overweight(TreeNode* node) {
        if (GetHeight(node->left) - GetHeight(node->right) == 2) {
            return 1;
        } else if (GetHeight(node->right) - GetHeight(node->left) == 2) {
            return -1;
        }
        return 0;
    }

    void RecalcHeight(TreeNode* node) {
        node->height = std::max(GetHeight(node->left), GetHeight(node->right)) + 1;
    }
};

void GetInput(int& n_size, std::vector<Segment>& segments) {
    std::cin >> n_size;
    int x_coord[2], y_coord[2];
    for (int iter = 0; iter < 2 * n_size; iter += 2) {
        std::cin >> x_coord[0] >> y_coord[0] >> x_coord[1] >> y_coord[1];
        int min_x = std::min(x_coord[0], x_coord[1]);
        int max_x = std::max(x_coord[0], x_coord[1]);
        int min_y = std::min(y_coord[0], y_coord[1]);
        int max_y = std::max(y_coord[0], y_coord[1]);
        segments.emplace_back(min_x, max_x, min_y);
        segments.emplace_back(min_x, max_x, max_y);
    }
}

int Solve(int& n_size, std::vector<Segment>& segments) {
    auto comp = [](Segment one, Segment two) {
        if (one.y_coord == two.y_coord) {
            return one.x_right < two.x_right;
        }
        return one.y_coord < two.y_coord;
    };

    std::sort(segments.begin(), segments.end(), comp);

    AVLtree tree = AVLtree();

    int ans = 0;
    for (auto segment : segments) {
        int res = tree.InsertSegment(segment);
        if (res == 1) {
            ++ans;
        }
        else if (res == -1) {
            tree.RemoveSegment(segment);
        }
    }
    tree.ClearSubtree();
    return ans;
}

int main() {

    int n_size;    
    std::vector<Segment> segments;
    GetInput(n_size, segments);

    std::cout << Solve(n_size, segments);
    return 0;
}
