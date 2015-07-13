"""
Parser of srt command files
"""
import re
import logging

from Queue import Queue
from keywords import KEYWORDS, is_valid_keyword

logging.basicConfig()
log = logging.getLogger(__name__)
log.setLevel(logging.DEBUG)


# constants describing regex groups
CMD_WAIT = 1
CMD_KEY = 2
CMD_ARGS = 3
WAIT = 5


class CommandFileParser(object):
    """
    Parses command files and creates an execution queue which corresponds
    to the commands specified in the file.

    Command file supported keywords (MIT Haystack strn compatible):
      * sourcename      * mode
      * radec           * azel
      * galactic        * offset
      * stow            * calibrate
      * noisecal        * record
      * freq            * roff
      * wait

    """
    def __init__(self):
        self.regex = re.compile("(^:([0-9]+)?\s+?({})\s+?([^/\*:\n]*))|(^:([0-9]+))".format('|'.join(KEYWORDS)))
        self.task_queue = Queue()

    @property
    def command_queue(self):
        return self.task_queue

    def parse_file(self, filename):
        """
        Parses a specified file as a command file. Attempts to read the formatted
        data in that file and generate items for the task queue corresponding to
        any valid commands found in the file.

        :param filename:
        :return:
        """
        try:
            with open(filename, 'r') as f:
                for line in f.readlines():
                    res = self.regex.match(line)

                    # If there was a match
                    if res is not None:
                        self._process_match(res)

        except IOError as e:
            raise e  # for now, just raise the exception

    def _process_match(self, match_result):
        """
        Process the regex match results for a given command.

        The two forms a valid command line will take are either:

            : <wait time> <command> <args>
            :<wait time>

        :param match_result:
        :return:
        """
        groups = match_result.groups()

        # the command is a wait command with no specified keyword
        if groups[WAIT] and not groups[CMD_KEY]:
            command = 'wait'
            wait = None
            args = groups[WAIT]

        # the command is specified by a keyword
        else:
            command = groups[CMD_KEY]
            wait = groups[CMD_WAIT]
            args = groups[CMD_ARGS]

        log.debug((command, wait, args))

        # validate that the keyword is supported
        self._validate_command(command, wait, args)

    def _validate_command(self, command, wait, args):
        """
        Validate the command keyword is a supported keyword. If the
        command is successfully validated, the command and its args
        will be added to the class's task_queue.

        :param command:
        :param wait:
        :param args:
        :return:
        """
        if is_valid_keyword(command):
            args = filter(None, args.split(' '))
            self.task_queue.put((command, wait, args))
        else:
            log.error('Invalid keyword while parsing command file: {}'.format(command))


if __name__ == "__main__":
    fr = CommandFileParser()
    fr.parse_file('cmd.txt')
