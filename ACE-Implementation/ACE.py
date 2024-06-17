import numpy as np
import pandas as pd

# Constants
total_nodes = 100
iterations = 10
cluster_heads_per_iteration = total_nodes // 20

# Initialize the network
nodes = pd.DataFrame({
    'NodeID': range(total_nodes),
    'State': ['Follower'] * total_nodes,
    'ClusterID': [-1] * total_nodes
})

def select_cluster_heads(nodes, num_heads):
    # Reset all nodes to follower
    nodes['State'] = 'Follower'
    nodes['ClusterID'] = -1
    
    # Randomly select cluster heads
    cluster_heads = np.random.choice(nodes['NodeID'], num_heads, replace=False)
    nodes.loc[cluster_heads, 'State'] = 'ClusterHead'
    nodes.loc[cluster_heads, 'ClusterID'] = cluster_heads

def assign_followers(nodes):
    # Assign each follower to a random cluster head
    cluster_heads = nodes[nodes['State'] == 'ClusterHead']['NodeID']
    followers = nodes[nodes['State'] == 'Follower']
    
    for idx in followers.index:
        assigned_head = np.random.choice(cluster_heads)
        nodes.at[idx, 'ClusterID'] = assigned_head

for iteration in range(iterations):
    select_cluster_heads(nodes, cluster_heads_per_iteration)
    assign_followers(nodes)
    
    # Print iteration result
    print(f"Iteration: {iteration + 1}")
    print(nodes)
    print(nodes.groupby('ClusterID').size().reset_index(name='Followers'))

