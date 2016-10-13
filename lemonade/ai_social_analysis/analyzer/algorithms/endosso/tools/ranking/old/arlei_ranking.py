# -*- coding: utf-8 -*-
from collections import defaultdict
from scipy.sparse import coo_matrix
import operator

def power_method(M1, M2, num_iterations, damping_factor, num_columns, user=""):
	""" Calculates the Page Rank iteratively using power method.
	
		In mathematics, the power iteration is an eigenvalue algorithm: given a matrix A, the algorithm 
		will produce a number λ (the eigenvalue) and a nonzero vector v (the eigenvector), such that Av = λv. 
		The algorithm is also known as the Von Mises iteration. 
	
		Arguments:
		M1: Matrix user-content
		M2: Matrix content-user
		num_iterations: Maximum number of iterations in power method eigenvalue solver.
		damping_factor: The PageRank theory holds that even an imaginary surfer who is randomly clicking on links will 
			eventually stop clicking. The probability, at any step, that the person will continue is a damping factor d. 
			Various studies have tested different damping factors, but it is generally assumed that the damping factor 
			will be set around 0.85 
		num_columns: TODO
		user: TODO
	"""
	row, column, data = [], [], []

	if user == "":
		for i in xrange(0, num_columns):
			row.append(0)
			column.append(i)
			data.append(float(1) / num_columns)
	else:
		for i in xrange(0, num_columns):
			row.append(0)
			column.append(i)
		
			if user == i:
				data.append(float(1))
			else:
				data.append(0)

	C = coo_matrix((data, (row, column)), shape=(1, num_columns))
	U = C

	#M = M1 * M2

	for i in xrange(num_iterations):
		#S = C * M
		T = C * M1
		S = T * M2
			
		S = damping_factor * S + (1.0 - damping_factor) * U

		#	e = sqeuclidean(S.getrow(0).todense(),C.getrow(0).todense())
	
		C = S

	return S.getrow(0).toarray()

def read_data(input_file_name):
	''' 
	Reads data from a CSV file. Each line has the format:
	user_name,tweet_id,tweet_date
	The field tweet_date is formated as %Y%m%d%H%M%S.
	'''
	user_id, content_id = 0, 0

	contents, users = {}, {}
	user_content, content_user, non_dangling_users = [], [], {}
	sum_user = defaultdict(int)
	
	content_to_user = {}
	user_photo = {}
	with open(input_file_name, 'r') as input_file:
		for line in input_file:
			user_name, content, data, rts, photo = line.rsplit('\t') #@UnusedVariable
			user_photo[user_name] = photo
			content_to_user[content] = user_name
			
			if user_name not in users:
				users[user_name] = user_id
				user = user_id
				user_id = user_id + 1
			else:
				user = users[user_name]
				
			if content not in contents:
				contents[content] = content_id
				content_user.append([content_id, user, 1])
				content_id = content_id + 1
			else:
				non_dangling_users[user] = 1
			
			content = contents[content]
			user_content.append([user, content, 1])
			sum_user[user] = sum_user[user] + 1

	for user in sum_user:
		content_user.append([content_id, user, float(1) / user_id])

		if user not in non_dangling_users:
			user_content.append([user, content_id, 1])
			sum_user[user] = sum_user[user] + 1

	for i in xrange(0, len(user_content)):
		user_content[i][2] = float(user_content[i][2]) / (sum_user[user_content[i][0]])

	content_id += 1

	row, column, data = [], [], []

	for i in xrange(0, len(user_content)):
		row.append(user_content[i][0])
		column.append(user_content[i][1])
		data.append(user_content[i][2])

	user_content = []

	UC = coo_matrix((data, (row, column)), shape=(user_id, content_id))
	
	row, column, data = [], [], []
	for i in xrange(0, len(content_user)):
		row.append(content_user[i][0])
		column.append(content_user[i][1])
		data.append(content_user[i][2])

	CU = coo_matrix((data, (row, column)), shape=(content_id, user_id))

	return users, contents, UC, CU, content_to_user, user_photo

def compute_relevance(input_file, num_iterations, damping_factor, user):
	(users, contents, UC, CU, content_to_user, user_photo) = read_data(input_file)
	
	if user != "" and user in users:
		relevance_vector = power_method(UC, CU, num_iterations, damping_factor, len(users), users[user])
	else:
		relevance_vector = power_method(UC, CU, num_iterations, damping_factor, len(users))
	
	relevance = {}

	for user in users:
		relevance[user] = relevance_vector[0][users[user]]

	user_relevance = sorted(relevance.iteritems(), key=operator.itemgetter(1), reverse=True)

	if user == "" or user not in users:
		relevance_vector = power_method(CU, UC, num_iterations, damping_factor, len(contents) + 1)
	else:
		relevance_vector = relevance_vector * UC

	relevance = {}

	for content in contents:
		relevance[content] = relevance_vector[0][contents[content]] + (float(relevance_vector[0][len(relevance_vector[0]) - 1]) / len(contents))

	content_relevance = sorted(relevance.iteritems(), key=operator.itemgetter(1), reverse=True)

	return user_relevance, content_relevance, content_to_user, user_photo

def compute_user_statistics(input_file_name):
	user_num_retweets = {}
	user_average_num_retweets = {}
	user_num_retweeters = {}
	user_sum_num_retweets_retweeter = {}

	content_user = {}
	num_tweets = {}
	user_retweeter = {}
	
	input_file = open(input_file_name, 'r')
	
	for line in input_file:
		line = line.rstrip()
		vec = line.rsplit(',')
		user = vec[0]
		content = vec[1]

		if user not in num_tweets:
			user_num_retweets[user] = 0
			user_average_num_retweets[user] = 0
			num_tweets[user] = 0

		if content not in content_user:
			content_user[content] = user
			num_tweets[user] = num_tweets[user] + 1
		else:
			user_num_retweets[content_user[content]] = user_num_retweets[content_user[content]] + 1
			user_retweeter[content_user[content] + "+" + user] = 1
	
	input_file.close()

	for pair in user_retweeter:
		vec = pair.rsplit('+')
		user = vec[0]
		retweeter = vec[1]
	
		if user in user_num_retweeters:
			user_num_retweeters[user] = user_num_retweeters[user] + 1
		else:
			user_num_retweeters[user] = 1
	
		if user in user_sum_num_retweets_retweeter:
			user_sum_num_retweets_retweeter[user] = user_sum_num_retweets_retweeter[user] + user_num_retweets[retweeter]
		else:
			user_sum_num_retweets_retweeter[user] = user_num_retweets[retweeter]
		
	for user in user_num_retweets:
		if num_tweets[user] > 0:
			user_average_num_retweets[user] = float(user_num_retweets[user]) / num_tweets[user]
		if user not in user_num_retweeters:
			user_num_retweeters[user] = 0
		if user not in user_sum_num_retweets_retweeter:
			user_sum_num_retweets_retweeter[user] = 0

	return (user_num_retweets, user_average_num_retweets, user_num_retweeters, user_sum_num_retweets_retweeter) 

def compute_content_statistics(input_file_name):
	content_num_retweets = {}
	content_sum_num_retweets_retweeter = {}
	user_num_retweets = {}
	
	content_user = {}
	
	input_file = open(input_file_name, 'r')
	
	for line in input_file:
		line = line.rstrip()
		vec = line.rsplit(',')
		user = vec[0]
		content = vec[1]

		if user not in user_num_retweets:
			user_num_retweets[user] = 0
		
		if content not in content_num_retweets:
			content_num_retweets[content] = 0
			content_user[content] = user
		else:
			content_num_retweets[content] = content_num_retweets[content] + 1
			user_num_retweets[content_user[content]] = user_num_retweets[content_user[content]] + 1

	input_file.close()
	
	input_file = open(input_file_name, 'r')
	
	for line in input_file:
		line = line.rstrip()
		vec = line.rsplit(',')
		user = vec[0]
		content = vec[1]
	
		if content not in content_sum_num_retweets_retweeter:
			content_sum_num_retweets_retweeter[content] = 0
	
		if user != content_user[content]:
			content_sum_num_retweets_retweeter[content] = content_sum_num_retweets_retweeter[content] + user_num_retweets[user]

	input_file.close()


	return (content_num_retweets, content_sum_num_retweets_retweeter, content_user)

