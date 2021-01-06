EIGEN_PATH = /home/inada/Documents/eigen-3.3.9

app: main.cpp index_tree.cpp encrypted_index_tree.cpp data_type.cpp
	gcc -I $(EIGEN_PATH) main.cpp index_tree.cpp encrypted_index_tree.cpp data_type.cpp -lstdc++

	
