/***************************************************************
 *
 * iOSChatClientViewController.m
 * iOSChatClient
 *
 ***************************************************************
 *
 * This software was written in 2013 by the following author(s):
 * Brendan A R Sechter <bsechter@sennue.com>
 *
 * To the extent possible under law, the author(s) have
 * dedicated all copyright and related and neighboring rights
 * to this software to the public domain worldwide. This
 * software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain
 * Dedication along with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 *
 * Please release derivative works under the terms of the CC0
 * Public Domain Dedication.
 *
 ***************************************************************/

// module headers
#import "iOSChatClientDefaults.h"
#import "iOSChatClientViewController.h"

// interface
@interface iOSChatClientViewController ()

@end

// implementation
@implementation iOSChatClientViewController

@synthesize chatViewTableView, chatViewButton, chatViewTextField;

- (void)viewDidLoad
{
    [super viewDidLoad];
	[self initNetworkCommunication];
    messages = [[NSMutableArray alloc] init];
    loginComplete = false;
    userid        = 0;
    [self relabelButton:@"Join"];
    //[self.chatViewTableView setHidden:YES];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // dispose of resources that can be recreated
}

- (void)dealloc
{
}

- (void)initNetworkCommunication
{
    // load settings
    NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
    port = [defaults integerForKey:IOS_CHAT_CLIENT_DEFAULT_KEY_PORT];
    host = [defaults stringForKey: IOS_CHAT_CLIENT_DEFAULT_KEY_HOST];
    
    // pair streams to socket
    CFReadStreamRef  readStream;
    CFWriteStreamRef writeStream;
    CFStreamCreatePairWithSocketToHost(NULL, (__bridge CFStringRef)host, port, &readStream, &writeStream);
    
    // initialize input stream
    inputStream  = (NSInputStream *) CFBridgingRelease(readStream);
    [inputStream  setDelegate:self];
    [inputStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [inputStream open];

    // initialize output stream
    outputStream = (NSOutputStream *)CFBridgingRelease(writeStream);
    [outputStream setDelegate:self];    
    [outputStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [outputStream open];
}

- (void)relabelButton:(NSString *)pLabel
{
    [chatViewButton setTitle:pLabel forState:UIControlStateNormal];
}

- (void)sendMessage:(NSString *)pMessage
{
	NSData * data = [[NSData alloc] initWithData:[pMessage dataUsingEncoding:NSASCIIStringEncoding]];
	[outputStream write:[data bytes] maxLength:[data length]];
}

- (void)receiveMessage:(NSString *)pMessage
{
    NSArray * components = [pMessage componentsSeparatedByString:@"/"];
    if ([components count] < 2)
    {
        return;
    }
    NSString * command = [components objectAtIndex:1];
    if ([command isEqualToString:@"login"] && (5 <= [components count]))
    {
        // NSInteger senderId = [(NSString *)[components objectAtIndex:2] intValue];
        userid             = [(NSString *)[components objectAtIndex:3] intValue];
        username           =  (NSString *)[components objectAtIndex:4];
        NSString * message = [NSString stringWithFormat:@"User ID : %d", userid];
        [messages insertObject:message atIndex:0];
    }
    if ([command isEqualToString:@"logout"])
    {
        exit(EXIT_SUCCESS);
    }
    if ([command isEqualToString:@"username"] && (5 <= [components count]))
    {
        // NSInteger senderId = [(NSString *)[components objectAtIndex:2] intValue];
        NSInteger receiverId = [(NSString *)[components objectAtIndex:3] intValue];
        if (userid == receiverId)
        {
            username           =  (NSString *)[components objectAtIndex:4];
            NSString * message = [NSString stringWithFormat:@"User Name : %@", username];
            [messages insertObject:message atIndex:0];
            [self relabelButton:@"Send"];
            loginComplete = true;
        }
    }
    if ([command isEqualToString:@"message"] && (4 <= [components count]))
    {
        NSInteger  senderId   = [(NSString *)[components objectAtIndex:2] intValue];
        NSString * rawMessage =  (NSString *)[components objectAtIndex:3];
        NSString * message = [NSString stringWithFormat:@"User %d : %@", senderId, rawMessage];
        [messages insertObject:message atIndex:0];
    }
    [self.chatViewTableView reloadData];
}

- (IBAction)joinChat:(id)pSender
{
    NSString * message = [NSString stringWithFormat:@"/username/%1$d/%1$d/%2$@", userid, chatViewTextField.text];
    [self sendMessage:message];
    //[self.chatViewTableView setHidden:NO];
}

- (IBAction)sendChatMessage:(id)pSender
{
    NSString * message = [NSString stringWithFormat:@"/message/%d/%@", userid, chatViewTextField.text];
    [self sendMessage:message];
}

- (IBAction)onChatViewButtonPress:(id)pSender
{
    loginComplete ? [self sendChatMessage:pSender] : [self joinChat:pSender];
    [chatViewTextField setText:@""];
}

- (BOOL)textFieldShouldReturn:(UITextField *)pTextField
{
    [self onChatViewButtonPress:nil];
    return YES;
}

- (void)stream:(NSStream *)pStream handleEvent:(NSStreamEvent)pStreamEvent
{
    NSString * eventDescription = nil;
    switch (pStreamEvent)
    {
        case NSStreamEventOpenCompleted:
            if (pStream == inputStream)
            {
                [messages insertObject:@"Connected" atIndex:0];
                [messages insertObject:[[NSString alloc] initWithFormat:@"Host %@ : Port %d", host, port]
                               atIndex:0];
                eventDescription = [[NSString alloc] initWithFormat:@"Connected : Host %@ : Port %d", host, port];
            }
            break;
        case NSStreamEventHasBytesAvailable:
            if (pStream == inputStream)
            {
                uint8_t buffer[IOS_CHAT_CLIENT_NETWORK_MESSAGE_SIZE];
                int     length;
                
                while ([inputStream hasBytesAvailable])
                {
                    length = [inputStream read:buffer maxLength:sizeof(buffer)];
                    if (0 < length)
                    {
                        NSString * message = [[NSString alloc] initWithBytes:buffer length:length encoding:NSASCIIStringEncoding];
                        if (nil != message)
                        {
                            eventDescription = [[NSString alloc] initWithFormat:@"Message : %@", message];
                            [self receiveMessage:message];
                        }
                    }
                }
            }
            break;
        case NSStreamEventErrorOccurred:
            if (pStream == inputStream)
            {
                eventDescription = @"Error";
                [messages insertObject:eventDescription atIndex:0];
            }
            break;
        case NSStreamEventEndEncountered:
            if (pStream == inputStream)
            {
                eventDescription = @"Disconnected";
                [messages insertObject:eventDescription atIndex:0];
            }
            break;
        default:
            eventDescription = @"Unknown Event";
            break;
    }
    if (nil != eventDescription)
    {
        NSLog(@"Stream Event : %@", eventDescription);
    }
    [self.chatViewTableView reloadData];
}

- (UITableViewCell *)tableView:(UITableView *)pTableView cellForRowAtIndexPath:(NSIndexPath *)pIndexPath
{
    static NSString * CellIdentifier = @"ChatCellIdentifier";
	UITableViewCell * cell           = [pTableView dequeueReusableCellWithIdentifier:CellIdentifier];
    NSString *        text           = (NSString *)[messages objectAtIndex:pIndexPath.row];
    if (nil == cell)
    {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
    }
    cell.textLabel.text = text;
	return cell;
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
	return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
	return messages.count;
}

@end
