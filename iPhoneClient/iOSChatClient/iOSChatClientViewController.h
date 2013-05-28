/***************************************************************
 *
 * iOSChatClientViewController.h
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

// library headers
#import <UIKit/UIKit.h>

// interface
@interface iOSChatClientViewController : UIViewController <NSStreamDelegate, UITextFieldDelegate, UITableViewDelegate, UITableViewDataSource>
{
    NSString *       host;
    NSInteger        port;
    NSInputStream *  inputStream;
    NSOutputStream * outputStream;
    NSMutableArray * messages;
    BOOL             loginComplete;
}

@property (weak, nonatomic) IBOutlet UITextField * chatViewTextField;
@property (weak, nonatomic) IBOutlet UIButton *    chatViewButton;
@property (weak, nonatomic) IBOutlet UITableView * chatViewTableView;

@end
