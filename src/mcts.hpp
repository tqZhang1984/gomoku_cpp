#include<vector>
#include<cmath>
#include<utility>
#include<random>
#include<algorithm>
#include<stack>
#include<set>

const int simulation_num = 20000;
const double k_coef = 1;

enum class PiecePlayer { human, computer, nobody};

struct node{
    PiecePlayer player;
    std::vector < node * > children;
    node* parent;
    int wins;
    int visits;
    std::pair<int, int> pos;
};

void print_tree(node* root){
	if(root->children.empty()){
		return;
	}
	for(auto child : root->children){
		printf("child: %d %d, child pos:%d %d\n", child->pos.first, child->pos.second, child->pos.first, child->pos.second);
		print_tree(child);
	}
}

PiecePlayer switch_player(PiecePlayer player){
	if(player == PiecePlayer::computer){
		return PiecePlayer::human;
	}else if(player == PiecePlayer::human){
		return PiecePlayer::computer;
	}else{
		return PiecePlayer::nobody;
	}
}

class mcts{
    friend class Gomoku;
public:
    mcts(std::vector<std::vector< int>> board_, bool player_ = true){
        board.resize(board_.size(), std::vector<PiecePlayer>(board_[0].size(), PiecePlayer::nobody));
		for(int i=0; i<board_.size(); i++){
			for(int j=0; j<board_[0].size(); j++){
				if(board_[i][j] == 2){
					board[i][j] = PiecePlayer::human;
				}else if(board_[i][j] == 1){
					board[i][j] = PiecePlayer::computer;
				}else{
					board[i][j] = PiecePlayer::nobody;
				}
			}
		}
        if(player_ == true){
			player = PiecePlayer::computer;
		}else{
			player = PiecePlayer::human;
		}
        board_size = std::make_pair(board.size(), board[0].size());
    }
private:
    std::vector<std::vector< PiecePlayer>> board;
    std::pair<int, int> board_size;
    PiecePlayer player;
    std::stack< std::pair<int, int>> path;
    std::pair<int, int> run();
    bool empty();
    node* selection(node* root);
    node* expansion(node* root);
    int rollout(node* root);
    void backpropagation(node* root, int result);
    void deleteTree(node* root){
        	if (!root->children.empty())
        {
            int size = root->children.size();
            for (int i=0; i<size; i++)
                deleteTree(root->children[i]);
        }
        delete(root);
    }
	bool possible_move(int col, int row);
	std::vector<std::pair<int, int>> get_avaliable();
    bool check_win();
};

double score(const node* root){
	if(root->visits == 0){
		return INFINITY;
	}
    return ((double)root->wins / root->visits) + k_coef*std::sqrt(std::log(root->parent->visits) / root->visits);
}

node* mcts::selection(node* root){
	node *curr_node;
	curr_node = root;
	while (curr_node->visits > 0) //如果当前节点被访问过，就继续往下找
	{	
        if(curr_node->children.empty()) break; //如果到了叶子节点就要停止
		curr_node = *std::max_element(
				curr_node->children.begin(),
				curr_node->children.end(),
				[](const node *a, const node *b)
				{
					double x = score(a);
					double y = score(b);
					return x < y;
				}
		);
		//printf("curr node: %d %d\n", curr_node->pos.first, curr_node->pos.second);
		path.push(std::make_pair(curr_node->pos.first, curr_node->pos.second));
		board[curr_node->pos.first][curr_node->pos.second] = curr_node->player;
	}
	return curr_node;
}

bool mcts::possible_move(int row, int col){
	for(int i=row-2; i<=row+2; i++){
		for(int j=col-2; j<=col+2; j++){
			if(i>=0 && i<board_size.first && j>=0 && j<board_size.second && board[i][j] != PiecePlayer::nobody){
				return true;
			}
		}
	}
	return false;
}

node* mcts::expansion(node* root){
	PiecePlayer curr_player = switch_player(root->player);
	std::vector< std::pair<int, int> > moves = get_avaliable();
	if (moves.empty())
		return root;
	int moves_size = moves.size();
	// 创建子节点
	for (int i=0; i<moves_size; i++)
	{
		node *newNode = new node;
		newNode->player= curr_player;
		newNode->pos.first = moves[i].first;
		newNode->pos.second = moves[i].second;
		root->children.emplace_back(newNode);
        newNode->parent = root;
        newNode->wins = 0;
        newNode->visits = 0;
	}
	//printf("expansion finish\n");
	// 随机选择一个子节点
	std::uniform_int_distribution<unsigned> dis(0, moves_size-1);
	std::random_device rd;
	std::mt19937 gen(rd());
	int num = dis(gen);
	//printf("choson num is :%d\n, children size is: %d", num, root->children.size());
	node* chosen_node = root->children[num];
	//printf("chosen node: %d %d\n", chosen_node->pos.first, chosen_node->pos.second);
	path.push(std::make_pair(chosen_node->pos.first, chosen_node->pos.second));
	board[chosen_node->pos.first][chosen_node->pos.second] = chosen_node->player;
	//printf("expansion: %d %d\n", chosen_node->pos.first, chosen_node->pos.second);
	return chosen_node;
}

std::vector<std::pair<int, int>> mcts::get_avaliable(){
	std::vector< std::pair<int,int>> avaliable;
	for(int i=0; i<board_size.first; i++){
		for(int j=0; j<board_size.second; j++){
			if(possible_move(i, j) && board[i][j] == PiecePlayer::nobody){
				avaliable.emplace_back(std::make_pair(i, j));
			}
		}
	}
	return avaliable;
}

int mcts::rollout(node* root){
    if(check_win()){
        return root->player==PiecePlayer::computer ? 2 : 1;
    }
	std::vector<std::pair<int, int>> available = get_avaliable();
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(available.begin(), available.end(), g);
	// 随机走
	PiecePlayer playerNow = switch_player(root->player);
	while (!available.empty())
	{
		board[available[0].first][available[0].second] = playerNow;
		path.push(std::make_pair(available[0].first, available[0].second));
		if (check_win())
		{
			return root->player==PiecePlayer::computer ? 2 : 1;
		}
		playerNow = switch_player(playerNow);
		available.erase(available.begin());
	}
	return 0;
}

void mcts::backpropagation(node* root, int result){
	while (!path.empty())
	{
		board[path.top().first][path.top().second] = PiecePlayer::nobody;
		path.pop();
	}
	while(root != nullptr){
		root->visits++;
		if(result == 0){
			root->wins += 0.5;
		}else if(result == 1){
			if(root->player == PiecePlayer::human){
				root -> wins += 1;
			}
		}else if(result == 2){
			if(root->player == PiecePlayer::computer){
				root -> wins += 1;
			}
		}
		root = root->parent;
	}
}

std::pair<int,int> mcts::run(){
	std::srand(30);

    if(empty()){
        return std::make_pair(board_size.first/2, board_size.second/2);
    }
	std::vector< std::pair<int, int> > possible_move = get_avaliable();
    node *root = new node;
	root->player = PiecePlayer::nobody;
	root->pos = std::make_pair(-1,-1);
	root -> parent = nullptr;
	root -> visits = 1;
	if(possible_move.size() == 1){
		return std::make_pair(possible_move[0].first, possible_move[0].second);
	}
	for(auto pairs : possible_move){
		node *newNode = new node;
		newNode->player = player;
		newNode->pos = pairs;
		newNode->parent = root;
		newNode->wins = 0;
		newNode->visits = 0;
		root->children.emplace_back(newNode);
	}
	int cnt = 0;
	for(int i=0; i<simulation_num; i++){
		node *leaf = selection(root);
		cnt ++;
		if(leaf->visits > 0 && leaf->children.empty()){
			leaf = expansion(leaf);
		}
		//printf("next leaf: %d %d\n", leaf->pos.first, leaf->pos.second);
		int result = rollout(leaf);
		backpropagation(leaf, result);
	}
	node *best_action = *std::max_element(
		root->children.begin(),
		root->children.end(),
		/*[](const node *a, const node *b)
		{
			double x = score(a);
			double y = score(b);
			return x < y;
		}*/
		[](const node *a, const node *b)
		{
			return a->visits < b->visits;
		}
	);
	return std::make_pair(best_action->pos.first, best_action->pos.second);
}

bool mcts::empty(){
	for(int i=0; i<board_size.first; i++){
		for(int j=0; j<board_size.second; j++){
			if(board[i][j] != PiecePlayer::nobody){
				return false;
			}
		}
	}
	return true;
}

bool mcts::check_win(){
    const auto last_position = path.top();
    const PiecePlayer piece = board[last_position.first][last_position.second];
	const std::vector< std::pair<int, int> > directions = {
		std::make_pair(0, 1),
		std::make_pair(0, -1),
		std::make_pair(1, 0),
		std::make_pair(-1, 0),
		std::make_pair(-1, -1),
		std::make_pair(1, 1),
		std::make_pair(-1, 1),
		std::make_pair(1, -1)
	};
	for (int direction_index = 0; direction_index < 8;
			direction_index += 2) {
		std::vector<std::pair<int, int>> pieces;

		auto add_pieces_on_direction = [&](const std::pair<int, int> direction) {
			for (int i = 1; i <= 4; i++) {
				const std::pair<int, int> pos{last_position.first + direction.first * i,last_position.second + direction.second * i};
				if (pos.first < 0 || pos.first >= board_size.first || pos.second < 0
					|| pos.second >= board_size.second)
					break;

				if (board[pos.first][pos.second] == piece)
					pieces.emplace_back(std::make_pair(pos.first, pos.second));
				else
					break;
			}
		};

		add_pieces_on_direction(directions[direction_index]);
		add_pieces_on_direction(directions[direction_index + 1]);

		if (pieces.size() >= 4) {
			return true;
		}
	}
	return false;
}