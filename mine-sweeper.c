#include <stdio.h>
#include <stdlib.h>
#define F_D_BLUE        "\033[1;34m"
#define F_RED           "\033[1;31m"
#define RESET           "\033[0m"
#define F_GREEN         "\033[0;32m"
typedef struct cell{
	int x;
	int y;
	int is_mine;
	int neig_mines_count;
	int is_open;
	int is_flag;
} cell;
typedef struct cellfield {
	int rows;
	int cols;
	cell* cells;
} cellfield;

void clear() {
	system("clear");
	system("cls");
}
void printcell(cell cell) {
	if (cell.is_flag == 1) {		//if cell is flag
		printf(" F ");
		return;
	}
	if (cell.is_open == 1) {	
		if (cell.is_mine == 1) {	//if cell is open
			printf(F_RED " O " RESET);
			return;
		}
		if (cell.neig_mines_count == 0) {		//if cell is zero
			printf(" - ");
			return;
		}
		printf( " %d ", cell.neig_mines_count);
		return;
	}
	printf(F_D_BLUE " * " RESET);
	return;
}
int get_index(cellfield* field, int x, int y) {
	if (x < 0 || x >= field->rows || y < 0 || y >= field->cols) {
		return -1;
	}
	return x * field->cols + y;
}
void printfield(cellfield* field) {
	int index_i, index_j;
	for (index_i = 0; index_i < field->cols; index_i++) {
		if (index_i == 0) {
			printf("y \t  %d ", index_i);
		}
		else if(index_i<10) {
			printf(" %d ", index_i);
		}
		else {
			printf("%d ", index_i);
		}
	}
	printf("\nx\n");
	for (index_i = 0; index_i < field->rows; index_i++) {
		for (index_j = 0; index_j < field->cols; index_j++) {
			if (index_j == 0) {
				printf("%d\t ", index_i);
			}
			int index=index_i*field->cols+index_j;
			printcell(field->cells[index]);
		}
		printf("\n");
	}
}

cellfield* createfield(int rows, int cols, int mine_count) {
	int index_i,index_j;
	srand(time(0));
	cellfield* field=(cellfield*)malloc(sizeof(cellfield));
	field->rows=rows;
	field->cols=cols;
	field->cells=(cell*)malloc(rows*cols*sizeof(cell));
	for(index_i=0;index_i<rows;index_i++)
	{
		for(index_j=0;index_j<cols;index_j++)
		{
			int index=get_index(field,index_i,index_j);
			int mines=(rand()%(rows*cols))<mine_count;
			field->cells[index].x=index_i;
			field->cells[index].y=index_j;
			field->cells[index].is_flag=0;
			field->cells[index].neig_mines_count=0;
			field->cells[index].is_mine=mines;
			field->cells[index].is_open=0;
		}
	}
	for (index_i = 0; index_i < rows; index_i++) {
		for (index_j = 0; index_j < cols; index_j++) {
			int index=get_index(field,index_i,index_j);
			int count=neighbour_mines_count(field, index_i, index_j);
			field->cells[index].neig_mines_count=count;
		}
	}
	return field;
}
int neighbour_mines_count(cellfield* field, int x, int y) {
	int index_i,index_j,index,mines=0;
	index=get_index(field,x,y);
	if(field->cells[index].is_mine==1)	
	{
		return -1;
	}
	for(index_i=x-1;index_i<=x+1;index_i++)
	{
		for(index_j=y-1;index_j<=y+1;index_j++)
		{
			int neighbour_index=get_index(field,index_i,index_j);
			if(neighbour_index!=-1 && neighbour_index!=index)
			{
				if(field->cells[neighbour_index].is_mine==1)
				{
					mines++;
				}
			}
		}
	}
	return mines;
}
void open_zeros(cellfield* field, int x, int y) {
	int index_i, index_j;
	int index = get_index(field,x,y);
	for (index_i = x - 1; index_i <= x + 1; index_i++) {
		for (index_j = y - 1; index_j <= y + 1; index_j++) {
			int neighbour_index = get_index(field,index_i,index_j);
			if (neighbour_index != -1 && neighbour_index != index) {
				if (field->cells[neighbour_index].is_open == 0 && field->cells[neighbour_index].is_flag == 0) {
					open_cell(field, index_i, index_j);
				}
			}
		}
	}
}
int open_cell(cellfield* field, int x, int y) {
	int index = get_index(field,x,y);
	if (field->cells[index].is_flag == 1) {
		return 0;
	}
	if (field->cells[index].is_open == 1) {
		return open_neighbours(field, x, y);
	}
	field->cells[index].is_open = 1;
	if (field->cells[index].neig_mines_count == 0) {
		open_zeros(field, x, y);
	}
	return field->cells[index].is_mine;
}
int open_neighbours(cellfield* field, int x, int y) {
	int index_i, index_j;
	int index = get_index(field,x,y);
	for (index_i = x - 1; index_i <= x + 1; index_i++) {
		for (index_j = y - 1; index_j <= y + 1; index_j++) {
			int neighbour_index = get_index(field,index_i,index_j);
			if (neighbour_index != -1 && neighbour_index != index) {
				if (field->cells[neighbour_index].is_open == 0 && field->cells[neighbour_index].is_flag == 0) {
					if (field->cells[neighbour_index].is_mine == 1) {
						return 1;
					}
					field->cells[neighbour_index].is_open = 1;
					if (field->cells[neighbour_index].neig_mines_count == 0) {
						open_zeros(field, index_i, index_j);
					}
				}
			}
		}
	}
	return 0;
}
int check_field_win(cellfield* field) {
	int index_i, index_j;
	for (index_i = 0; index_i < field->rows; index_i++) {
		for (index_j = 0; index_j < field->cols; index_j++) {
			int index = get_index(field,index_i,index_j);
			if (field->cells[index].is_mine == 0 && field->cells[index].is_open == 0) {
				return 0;
			}
		}
	}
	return 1;
}

void flag_cell(cellfield* field, int x, int y) {
	int index = get_index(field,x,y);
	if (field->cells[index].is_open == 1) {
		return;
	}
	if (field->cells[index].is_flag == 1) {
		field->cells[index].is_flag = 0;
	}
	else {
		field->cells[index].is_flag = 1;
	}
}
int deciding_cell(cellfield* field, int x, int y, char mode) {
	if (mode == 'o' || mode == 'O') {
		return open_cell(field, x, y);
	}
	else if (mode == 'f' || mode == 'F') {
		flag_cell(field, x, y);
	}
	return 0;
}
void open_all(cellfield* field) {
	int index_i, index_j;
	for (index_i = 0; index_i < field->rows; index_i++) {
		for (index_j = 0; index_j < field->cols; index_j++) {
			int index = get_index(field,index_i,index_j);
			field->cells[index].is_flag = 0;
			field->cells[index].is_open = 1;
		}
	}
}
void open_all_mines(cellfield* field) {
	int index_i, index_j;
	for (index_i = 0; index_i < field->rows; index_i++) {
		for (index_j = 0; index_j < field->cols; index_j++) {
			int index = get_index(field,index_i,index_j);
			if(field->cells[index].is_mine == 1) {
				field->cells[index].is_flag = 0;
				field->cells[index].is_open = 1;
			}
		}
	}
}
int main() {
	int row_size,col_size,mine_count,x,y;
	char mode=' ',again=' ';
	clear();
	printf("=== Minesweeper ===\n\n");
BEGINNING:
	printf("Row size->");
	scanf("%d",&row_size);
	printf("Column size->");
	scanf("%d",&col_size);
	printf("Mine count->");
	scanf("%d",&mine_count);
	if(row_size<1 || col_size<1  || mine_count<1)
	{
		clear();
		goto BEGINNING;
	}
	cellfield* field=createfield(row_size,col_size,mine_count);
	clear();

START:
	printfield(field);
	printf("x-point->");
	scanf("%d",&x);
	printf("y-point->");
	scanf("%d",&y);
	printf("Mode   (Open->O) or (Flag->F)");
	scanf(" %c",&mode);
	int result=deciding_cell(field,x,y,mode);
	int is_win=check_field_win(field);
	clear();
	if(result==1 || is_win==1)
	{
		goto END;
	}
	goto START;
END:
	if (is_win == 1) {
		open_all(field);
		printfield(field);
		printf(F_GREEN "you win! :)\n" RESET);
	}
	else {
		open_all_mines(field);
		printfield(field);
		printf(F_RED "game over :(\n" RESET);
	}

	printf("want to continue? (y/n)> ");
	scanf(" %c", &again);

	if(again == 'y' || again == 'Y') {
		clear();
		goto BEGINNING;
	}

	printf("bye\n");

	
}
