"""

"""
import re

# Constants describing regex groups
CMD_WAIT = 1
CMD_KEY = 2
CMD_ARGS = 3
WAIT = 5


class FileReader(object):
    """

    """
    def __init__(self):
        self.regex = re.compile("(^:([0-9]+)?\s+?(record|calibrate|galactic|freq|roff)\s+?([^/\*:]*))|(^:([0-9]+))")

    def parse_file(self, filename):
        try:
            with open(filename, 'r') as f:
                for line in f.readlines():
                    res = self.regex.match(line)
                    groups = res.groups()

                    ## should check here to make sure something matched. if not, pass over

                    if groups[WAIT] and not groups[CMD_KEY]:
                        # Pure wait with no other command
                        pass

                    else:
                        # command with keyword
                        pass




        except IOError as e:
            pass


if __name__ == "__main__":
    fr = FileReader()
    fr.parse_file('cmd.txt')
