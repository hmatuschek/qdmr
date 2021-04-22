def chunks(lst, n):
    """Yield successive n-sized chunks from lst."""
    ret = []
    for i in range(0, len(lst), n):
        ret.append(lst[i:i + n])
    return ret
