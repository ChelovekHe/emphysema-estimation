class intersperse:
    '''Generator that inserts a value before each element of an iterable
    '''
    def __init__(self, value, iterable):
        self.value = value
        self.iterable = iterable
        self.return_value = True
        
    def __iter__(self):
        return self
    
    def __next__(self):
        if self.return_value:
            self.next_value = next( self.iterable )
            r = self.value
        else:
            r = self.next_value
        self.return_value = not self.return_value
        return r
