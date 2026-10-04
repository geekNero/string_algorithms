def transform_string(s):
    """
    Step 1: Converts a sequence into an array of distances to previous occurrences.
    First occurrences are mapped to 0.
    """
    last_seen = {}
    s_prime = []
    
    for i, char in enumerate(s):
        if char in last_seen:
            # Record the absolute distance to the previous occurrence
            s_prime.append(i - last_seen[char])
        else:
            # First occurrence
            s_prime.append(0)
        # Update the last seen index for this character
        last_seen[char] = i
        
    return s_prime

def p_match_compare(s_prime, k, d):
    """
    Step 2: The O(1) comparison rule to replace standard character equality.
    Evaluates if s_prime[k+d] matches the prefix character at s_prime[d].
    """
    prefix_val = s_prime[d]
    window_val = s_prime[k + d]

    # Condition A: The previous occurrence happened INSIDE the prefix window
    if 0 < prefix_val <= d:
        # The substring must have the exact same distance to match the relative structure
        return window_val == prefix_val
        
    # Condition B: The previous occurrence is OUTSIDE the prefix window (or it's a first occurrence)
    else:
        # The substring character must also point outside its current window (or be a first occurrence)
        return window_val == 0 or window_val > d

def modified_z_algorithm(s_prime):
    """
    Step 3: The standard Z-algorithm utilizing the O(1) p-match comparison.
    """
    n = len(s_prime)
    Z = [0] * n
    L, R = 0, 0

    for i in range(1, n):
        if i > R:
            # Case 1: Outside the rightmost Z-box. Explicitly compare using p-match rule.
            d = 0
            while i + d < n and p_match_compare(s_prime, i, d):
                d += 1
            Z[i] = d
            if d > 0:
                L, R = i, i + d - 1
        else:
            # Inside the rightmost Z-box. Calculate the corresponding prefix index.
            k_prime = i - L
            
            # Case 2a: The copied Z-value stays strictly within the R boundary.
            if Z[k_prime] < R - i + 1:
                Z[i] = Z[k_prime]
                
            # Case 2b: The copied Z-value hits or exceeds the R boundary. Extend explicitly.
            else:
                d = R - i + 1
                while i + d < n and p_match_compare(s_prime, i, d):
                    d += 1
                Z[i] = d
                L, R = i, i + d - 1
                
    return Z

def find_p_matches(text, pattern):
    """
    Main execution pipeline to find all substrings of T that p-match P.
    """
    # Use a unique separator that will never match any character in the text or pattern.
    # None serves perfectly here as it won't map to string characters.
    combined = list(pattern) + [None] + list(text)
    
    # 1. Transform the entire combined sequence globally in O(|P| + |T|) time
    s_prime = transform_string(combined)
    
    # 2. Run the modified Z-algorithm in O(|P| + |T|) time
    Z = modified_z_algorithm(s_prime)
    
    # 3. Scan the Z-values corresponding to the text portion to find matches
    pattern_len = len(pattern)
    matches = []
    
    for i in range(pattern_len + 1, len(Z)):
        # If the p-match length equals the pattern length, we found a full match
        if Z[i] == pattern_len:
            # Convert the combined array index back to the original text index
            text_index = i - (pattern_len + 1)
            matches.append(text_index)
            
    return matches

# --- Example Execution ---
pattern = "aba"
text = "xyx zyz aab"
print(f"Matches found at indices: {find_p_matches(text, pattern)}")
# Expected output: [0, 4] (matching 'xyx' and 'zyz')
