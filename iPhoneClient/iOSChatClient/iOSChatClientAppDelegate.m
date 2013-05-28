/***************************************************************
 *
 * iOSChatClientAppDelegate.m
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

#import "iOSChatClientAppDelegate.h"
#import "iOSChatClientDefaults.h"

@implementation iOSChatClientAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // set default preferences
    NSString * host = IOS_CHAT_CLIENT_DEFAULT_HOST;
    NSNumber * port = [NSNumber numberWithInt:IOS_CHAT_CLIENT_DEFAULT_PORT];
    NSArray *  keys   = [NSArray arrayWithObjects:
                         IOS_CHAT_CLIENT_DEFAULT_KEY_HOST, IOS_CHAT_CLIENT_DEFAULT_KEY_PORT, nil];
    NSArray *  values = [NSArray arrayWithObjects:
                         host,                             port,                             nil];
    NSUserDefaults * defaults    = [NSUserDefaults standardUserDefaults];
    NSDictionary *   appDefaults = [NSDictionary dictionaryWithObjects:values forKeys:keys];
    [defaults registerDefaults:appDefaults];
    [defaults synchronize];
    return YES;
}
							
- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
